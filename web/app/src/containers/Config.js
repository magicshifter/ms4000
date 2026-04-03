import React, {Component} from 'react'
import PropTypes from 'prop-types'
import {connect} from 'react-redux'
import {
  configDownload, configUpload, configUpdate
} from '../actions/ms3000'

import protobufs from '../utils/protoBufLoader'
import pb from '../utils/protoBufLoader'
import {throttle} from "../utils/debounce";

import AutoInterface from '../components/AutoInterface/index'
import IconButton from  '../components/inputs/IconButton'

import './App.css';
import {faCloudUploadAlt, faCloudDownloadAlt, faSyncAlt} from "@fortawesome/free-solid-svg-icons/index";
import ErrorBox from "../components/outputs/ErrorBox";


class Config extends Component {
  constructor(props) {
    super(props)

    const ctx = this
    this.dispatchDebouncedPostShifterState = throttle(() => {
      const { dispatch } = ctx.props
      dispatch(configUpload())
    }, 500)
  }

  onChangeAutoInterface = (newState, theType) => {
    const { dispatch } = this.props
    dispatch(configUpdate(newState))

    if (this.refs.fastSync.checked)
      this.dispatchDebouncedPostShifterState()
  }

  handleRefreshClick = e => {
    e.preventDefault()
    const { dispatch } = this.props
    dispatch(configDownload())
  }

  handlePostClick = e => {
    e.preventDefault()
    this.dispatchDebouncedPostShifterState()
  }

  handleTestBuffer = e => {
    console.log("create", pb.MS4.create())
    const {shifterState } = this.props

    const testObj = shifterState
    var check = pb.MS4.verify(testObj);
    console.log("verified:", check, testObj)

    var bufferU8 = pb.MS4.encode(testObj).finish()

    //dumpU8(bufferU8)
    const decodedObj = pb.MS4.decode(bufferU8);

    console.log("after decoding:", decodedObj, bufferU8)
  }

  handleTestDataClick = e => {
    e.preventDefault()
    const { dispatch } = this.props
    var r = Math.floor(Math.random()*256)
    var g = Math.floor(Math.random()*256)
    var b = Math.floor(Math.random()*256)
    dispatch(configUpdate({
      networkName:"Testdate MS",
      modes: {
        beat: {
          beatMode: 1,
          sensitivity: 2,
        },
        light: {
          name: Math.random() < 0.4 ? "The Light" : Math.random() < 0.4 ? "Licht" : "MagicLight",
          color: {
            R: r, G: g, B: b
          },
          subMode: 2
        }
      }
    }))
  }

  render() {
    const {
      shifterState,
      isConfigUploading,
      configUploadError,
      isConfigDownloading,
      configDownloadError
    } = this.props

      return (
          <div style={{border: '2px solid yellow'}}>
            <div className="pure-menu pure-menu-horizontal" style={{paddingBottom: "0px"}}>
              <ul className="pure-menu-list">
                <IconButton icon={faSyncAlt} tooltip='download config' onClick={this.handleRefreshClick} rotate={isConfigDownloading}/>
                <IconButton icon={faCloudUploadAlt} tooltip='upload config' onClick={this.handlePostClick} rotate={isConfigUploading}/>
                <ErrorBox error={configUploadError}/>
                <ErrorBox error={configDownloadError} />
              </ul>
            </div>

            <div>
              <button onClick={this.handleTestDataClick}>
                Get TestData
              </button>

              <button onClick={this.handleTestBuffer}>
                Test Buffer
              </button>

              fast sync: <input ref="fastSync" type="checkbox" defaultChecked={true} />
            </div>

            <AutoInterface type={protobufs.MS4}
                           onChange={this.onChangeAutoInterface}
                           value={shifterState}
                           legend="MS3000 State"

            />

            <pre> {JSON.stringify(shifterState, null, 2) }</pre>

          </div>
      )
  }
}

const mapStateToProps = state => {
  const {
    shifterState,
    isConfigUploading,
    configUploadError,
    isConfigDownloading,
    configDownloadError,
  } = state.ms3000

  return {
    shifterState,
    isConfigUploading,
    configUploadError,
    isConfigDownloading,
    configDownloadError,
  }
}

export default connect(mapStateToProps)(Config)
