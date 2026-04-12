import React, {Component} from 'react'
import PropTypes from "prop-types";
import {equRGB, hexFromRGB, RGB, shadeRGB} from "../../utils/color"
import floodFill from "n-dimensional-flood-fill"

function toolSL(size) {
  return -Math.floor(size/2)
}

function toolSR(size) {
  return Math.ceil(size/2)
}

function getMousePos(canvas, evt) {
  var rect = canvas.getBoundingClientRect();
  return {
    x: evt.clientX - rect.left,
    y: evt.clientY - rect.top
  };
}


function copyTouch(touch) {
  return { identifier: touch.identifier, pageX: touch.pageX, pageY: touch.pageY, force: touch.force, radiusX: touch.radiusX };
}

export default class PixelCanvas extends Component {
  static propTypes = {
    width: PropTypes.number.isRequired,
    height: PropTypes.number.isRequired,
    tool: PropTypes.string.isRequired,
    toolSize: PropTypes.number.isRequired,
    color: PropTypes.object.isRequired,
    pixel: PropTypes.object.isRequired,
    onChange: PropTypes.func.isRequired,
    onPick: PropTypes.func.isRequired,
    onScroll: PropTypes.func.isRequired,
    scale: PropTypes.number.isRequired,
  }


  constructor(props) {
    super(props)

    this.size = 15
  }





  setupTouch = (el) => {
    this.touches = {};

    el.addEventListener("touchstart",   this.handleTouchStart, false);
    el.addEventListener("touchend",     this.handleTouchEnd, false);
    el.addEventListener("touchcancel",  this.handleTouchCancel, false);
    el.addEventListener("touchmove",    this.handleTouchMove, false);
  }

  disposeTouch = (el) => {
    this.touches = {};


    el.removeEventListener("touchstart",  this.handleTouchStart, false);
    el.removeEventListener("touchend",    this.handleTouchEnd, false);
    el.removeEventListener("touchcancel", this.handleTouchCancel, false);
    el.removeEventListener("touchmove",   this.handleTouchMove, false);
  }

  handleTouchStart = (evt) => {

    evt.preventDefault()
    this.useTool(evt, 'down')
  }

  handleTouchMove = (evt) => {

    evt.preventDefault();
    this.useTool(evt, 'move')
  }

  handleTouchEnd = (evt) => {
    evt.preventDefault();
    const touches = evt.changedTouches;

    for (var i = 0; i < touches.length; i++) {
      const touch = touches[i];
      delete this.touches[touch.identifier]
    }
  }

  handleTouchCancel = (evt) => {
    evt.preventDefault();
    const touches = evt.changedTouches;

    for (var i = 0; i < touches.length; i++) {
      const touch = touches[i];
      delete this.touches[touch.identifier]
    }
  }

  onResize = () => {
    const {div} = this.refs
    let dW = 600 // iv.clientWidth;
    let dH = 500 // div.clientHeight;

    //console.log("resize", dW, dH)

    this.canvas.width = dW
    this.canvas.height = dH

    const { width, height } = this.props

    const sW = Math.floor(dW/width)
    const sH = Math.floor(dH/height)
    const scale = Math.min(sW, sH)
    this.scale = scale


    // this.canvas.width = 500
    // this.canvas.height = 500

    this.drawPixel()
  }

  componentDidMount() {
    var c = this.refs.canvas
    this.canvas = c

    var ctx = c.getContext("2d");
    this.canvasContext = ctx

    this.setupTouch(c)

    window.addEventListener("resize", this.onResize, false);
    this.onResize();
  }

  componentWillUnmount() {
    window.removeEventListener("resize", this.onResize, false);

    var c = this.canvas
    this.disposeTouch(c)
    this.canvas = null
  }

  getPixel = (x, y) => {
    const { pixel, width, height } = this.props

    if (x < 0 || x >= width || y < 0 || y >= height)
      return null

    const idx = y * width + x
    const v =  pixel.get(idx)
    return v
  }

  drawPixel = () => {
    const { pixel, width, height } = this.props
    const ctx = this.canvasContext
    const { scale } = this

    var index = 0
    for (var y = 0; y < height; y++) {
      for (var x = 0; x < width; x++) {
        ctx.fillStyle= hexFromRGB(pixel.get(index));
        ctx.fillRect(x*scale,y*scale,scale-1,scale-1);
        index++;
      }
    }
  }

  forToolArea = (X, Y, fn) => {
    const { toolSize} = this.props
    for (var xx = toolSL(toolSize); xx < toolSR(toolSize); xx++) {
      for (var yy = toolSL(toolSize); yy < toolSR(toolSize); yy++) {
        const x = X + xx
        const y = Y + yy
        const rgb = this.getPixel(x,y)
        if (!rgb)
          continue
        fn(x, y, rgb)
      }
    }
  }

  drawTheTool = (x, y) => {
    const { color, tool } = this.props
    if (tool === 'draw' || tool === 'erase')
      this.forToolArea(x, y, this.drawTheToolFn)
    else {
      this.drawTheToolFnAlways(x, y, this.getPixel(x, y))
    }
  }

  drawTheToolFn = (x, y, rgb) => {
    const { color } = this.props

    const { scale } = this

    const ctx = this.canvasContext
    if (!equRGB(rgb, color)) {
      const shaded = shadeRGB(rgb)
      ctx.fillStyle = hexFromRGB(shaded);
      ctx.fillRect(x * scale, y * scale, scale - 1, scale - 1);
    }
  }

  drawTheToolFnAlways = (x, y, rgb) => {
    const { scale } = this

    const ctx = this.canvasContext
    const shaded = shadeRGB(rgb)
    ctx.fillStyle = hexFromRGB(shaded);
    ctx.fillRect(x * scale, y * scale, scale - 1, scale - 1);
  }

  componentDidUpdate(prevProps, prevState, snapshot) {
    this.onResize();
    //this.drawPixel()
  }

  getPos = (evt) => {
    let { width, height } = this.props
    const { scale, canvas } = this

    let p
    if (evt.changedTouches && evt.changedTouches.length > 0) {
      console.log("touch detect", evt)


      var rect = canvas.getBoundingClientRect();
      const touch = evt.changedTouches[0]
      p = {
        x: touch.pageX - rect.left,
        y: touch.pageY - rect.top
      }
      /*

function getMousePos(canvas, evt) {
  var rect = canvas.getBoundingClientRect();
  return {
    x: evt.clientX - rect.left,
    y: evt.clientY - rect.top
  };
}


function copyTouch(touch) {
  return { identifier: touch.identifier, pageX: touch.pageX, pageY: touch.pageY, force: touch.force, radiusX: touch.radiusX };
}
       */
    }
    else {
      p = getMousePos(this.refs.canvas, evt)
    }

    var px = Math.floor(p.x / scale)
    var py = Math.floor(p.y / scale)

    if (px < 0) px = 0
    else if (px >= width) px = width - 1
    if (py < 0) py = 0
    else if (py >= height) py = height - 1

    return {x: px, y: py}
  }


  static black = RGB(0,0,0)

  useDrawToolInternal = (evt, color) => {
    const { onChange, toolSize, width, height } = this.props
    const p = this.getPos(evt)

    //console.log("pixeling")

    const changes = []
    if (toolSize <= 1) {
      p.color = color
      changes.push(p)
    }
    else {
      this.forToolArea(p.x, p.y, (x, y) => {
        const change = {x, y, color}
        changes.push(change)
      })
    }

    onChange(changes, { usedColor: color })
  }

  useFillTool = (evt) => {
    const { onChange, color } = this.props
    var p = this.getPos(evt)

    const area = this.floodFill(p.x, p.y)

    const changes = []
    for (var i = 0; i < area.length; i++) {
      const pos = area[i]
      const change = { x: pos[0], y: pos[1], color }
      changes.push(change)
    }
    onChange(changes, { usedColor: color })
  }

  usePickTool = (evt) => {
    const { onPick } = this.props
    var p = this.getPos(evt)

    const color = this.getPixel(p.x, p.y)

    onPick(color)
  }

  useScrollTool = (evt, type) => {
    const { onScroll } = this.props
    var p = this.getPos(evt)

    if (type === 'move') {
      const lP = this.lastScrollPos
      const dx = p.x - lP.x
      const dy = p.y - lP.y
      if (dx !== 0 || dy !== 0) {
        onScroll({x:dx, y:dy})
      }
    }

    this.lastScrollPos = p
  }


  useTool = (evt, type) => {
    const { tool, color } = this.props
    switch (tool) {
      case "draw":
        this.useDrawToolInternal(evt, color)
        break;

      case "erase":
        this.useDrawToolInternal(evt, PixelCanvas.black)
        break;

      case "fill":
        this.useFillTool(evt)
        break;

      case 'pick':
        this.usePickTool(evt)
        break

      case 'scroll':
        this.useScrollTool(evt, type)
        break

      default:
        console.log("unknown tool", tool);

    }
  }

  onMouseDownCanvas = (evt) => {
    evt.preventDefault()
    this.useTool(evt, 'down')
  }

  onMouseMoveCanvas = (evt) => {
    //console.log("movin...")
    evt.preventDefault()
    var p = this.getPos(evt)
    if (evt.buttons) {
      this.useTool(evt, 'move')
    }
    else {
      this.drawPixel()
      this.drawTheTool(p.x, p.y)
    }
  }

  // clearing tool preview
  onMouseLeaveCanvas = (evt) => {
    evt.preventDefault()
    this.drawPixel()
  }

  render() {
    let { width, height } = this.props

    //style={{minWidth: '0', flex: "1 1 auto", backgroundColor:'gray', border: '2px solid blue'}}

    return (
      <div ref='div' style={{minWidth: '0', flex: "1 1 auto", backgroundColor:'gray', border: '2px solid blue'}}
           >
        <canvas ref="canvas"
                onMouseDown={this.onMouseDownCanvas}
                onMouseMove={this.onMouseMoveCanvas}
                onMouseLeave={this.onMouseLeaveCanvas}
                style={{border: "0px solid #FFFFFF", userSelect: "none"}}/>
      </div>
    )
  }

  // Floodfill
  floodFill = (x, y) => {
    const { width, height } = this.props

    const colorToFlood = this.getPixel(x, y)

    // Define our getter for accessing the data structure.
    const ctx = this
    const getter = function (x, y) {
      if (x >= 0 && x < width && y >= 0 && y < height) {
        const c = ctx.getPixel(x, y)
        return equRGB(c, colorToFlood) ? 1 : 0
      }
      else
        return false
    }

    // Flood fill over the data structure.
    const result = floodFill({
      getter: getter,
      seed: [x, y]
    });
    return result.flooded
  }
}
