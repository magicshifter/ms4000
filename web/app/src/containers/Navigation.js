import React, {Component} from 'react'
import PropTypes from 'prop-types'
import {connect} from 'react-redux'
import {FontAwesomeIcon} from '@fortawesome/react-fontawesome'
import {navigationSetLocation} from '../actions/navigation'


import './App.css';

import {
  faBars
} from '@fortawesome/free-solid-svg-icons'

const navStructure = [
  { name: "wifi" },
  { name: "config" },
  // TODO: generate from modes
  { name: "mode-Image" },
  { name: "mode-Light" },
  { name: "mode-MIDI" },
  { name: "socket-test" },
  { name: "help" },
]


class Navigation extends Component {
  static propTypes = {
    location :PropTypes.string,
    structure: PropTypes.array.isRequired,
    onChange: PropTypes.func.isRequired
  }

  onClickNav = (evt) => {
    const { onChange } = this.props
    const name = evt.currentTarget.dataset["name"]

    console.log("clicked Nav", name, evt.currentTarget)
    onChange(name)
  }

  render() {
    const { structure, location } = this.props

    const controls = []
    for (var i = 0; i < structure.length; i++) {
      const nav = structure[i]
      const itemClassName = nav.name === location ? "ms3000-menu-item ms3000-menu-item-active" : "ms3000-menu-item"

      controls.push(
        <li key={i} className={itemClassName}>
          <a href="#" onClick={this.onClickNav} data-name={nav.name}>
            {nav.name}
          </a>
        </li>)
    }

    return (
      <div className="m3000-menu-container">
        <ul ref={this.setupBurgerRef} className="m3000-menu-list">
          {controls}
        </ul>
         <FontAwesomeIcon icon={faBars} size="2x" onClick={ this.onClickBurger} className="ms3000-menu-burger"/>
      </div>
    )
  }

  setupBurgerRef = (el) => {
    this.burgerRef = el
    console.log("REF")
  }  

  onClickBurger = () => {
    this.burgerRef.classList.toggle("ms3000-menu-visible")
    console.log("Click")
  }
}


const mapStateToProps = state => {
  const { location } = state.navigation

  return {
    location,
    // TODO: autogen navstructure from modes
    structure: navStructure
  }
}

const mapDispatchToProps = dispatch => {
  return {
    onChange: location => {
      dispatch(navigationSetLocation(location))
    }
  }
}

export default connect(mapStateToProps, mapDispatchToProps)(Navigation)
