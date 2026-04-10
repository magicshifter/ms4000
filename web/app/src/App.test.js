import reducer from './reducers'

describe('store bootstrap', () => {
  it('creates the expected top-level state shape', () => {
    const state = reducer(undefined, { type: '@@INIT' })

    expect(state).toHaveProperty('ms3000')
    expect(state).toHaveProperty('fileSystem')
    expect(state).toHaveProperty('sidebar')
    expect(state).toHaveProperty('sockets')
    expect(state).toHaveProperty('pixelEditor')
    expect(state).toHaveProperty('navigation')
    expect(state.navigation.location).toBe('config')
  })
})
