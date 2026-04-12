import React, {Component} from 'react'
import PropTypes from 'prop-types'
import {connect} from 'react-redux'
import Navigation from './Navigation'

import './App.css';

const routeLoaders = {
  wifi: () => import('./WiFiConfig').then((module) => [
    { key: 'soc', Component: module.default },
  ]),
  'socket-test': () => import('./SocketCmdCenter').then((module) => [
    { key: 'soc', Component: module.default },
  ]),
  'mode-Image': () => Promise.all([
    import('./FilesSidebar'),
    import('./PixelEditor'),
  ]).then(([filesSidebarModule, pixelEditorModule]) => [
    { key: 'fS', Component: filesSidebarModule.default },
    { key: 'mImage', Component: pixelEditorModule.default },
  ]),
  config: () => import('./Config').then((module) => [
    { key: 'cfg', Component: module.default },
  ]),
  help: () => import('../components/IconTest').then((module) => [
    { key: 'icontest', Component: module.default },
  ]),
}

class App extends Component {
  static propTypes = {
    shifterState :PropTypes.object,
    location: PropTypes.string.isRequired,
  }

  constructor(props) {
    super(props)

    this.state = {
      loadedRoutes: {},
      loadingLocation: null,
      loadError: null,
    }
  }

  componentDidMount() {
    this.ensureRouteLoaded(this.props.location)
  }

  componentDidUpdate(prevProps) {
    if (prevProps.location !== this.props.location) {
      this.ensureRouteLoaded(this.props.location)
    }
  }

  ensureRouteLoaded(location) {
    if (!routeLoaders[location] || this.state.loadedRoutes[location]) {
      return
    }

    this.setState({
      loadingLocation: location,
      loadError: null,
    })

    routeLoaders[location]()
      .then((routeComponents) => {
        this.setState((prevState) => ({
          loadedRoutes: {
            ...prevState.loadedRoutes,
            [location]: routeComponents,
          },
          loadingLocation: prevState.loadingLocation === location ? null : prevState.loadingLocation,
        }))
      })
      .catch((loadError) => {
        this.setState({
          loadError,
          loadingLocation: null,
        })
      })
  }

  render() {
    const { location } = this.props
    const { loadedRoutes, loadingLocation, loadError } = this.state

    let controls
    if (!routeLoaders[location]) {
      controls = [<div key="uknw">MS3000 Error 404 Unknown location: {location}</div>]
    }
    else if (loadedRoutes[location]) {
      controls = loadedRoutes[location].map(({ key, Component }) => (
        <Component key={key}/>
      ))
    }
    else if (loadError) {
      controls = [<div key="load-error">Failed to load {location}: {loadError.toString()}</div>]
    }
    else {
      controls = [<div key="loading">Loading {loadingLocation || location}...</div>]
    }

    //
    // style={{width: '100%', height:'100%', display: 'flex', flexFlow: 'column'}}
    return (
      <div >
        <Navigation/>
        {controls}
      </div>
    )
  }
}

const mapStateToProps = state => {
  const { shifterState } = state.ms3000
  const { location } = state.navigation

  return {
    shifterState,
    location
  }
}

export default connect(mapStateToProps)(App)
