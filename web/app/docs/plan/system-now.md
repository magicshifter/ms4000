# System Now

Date: 2026-04-09

## Executive Summary

This app is a legacy Create React App frontend for operating an `MS3000` device. Its current working center is the image editor and config editor. The app uses React 16 class components, Redux, `redux-thunk`, protobuf-driven form generation, and a custom `MagicBitmap` image format. The socket/chat path still exists in the codebase but is effectively dormant because saga wiring is commented out at startup.

The app does build, but only with many lint warnings. The test suite currently fails immediately because the only test imports a non-existent module.

## What The App Is

- A single-page device control app with hand-rolled navigation in [`src/containers/App.js`](/home/wizard/projects/asimov/MS4000/web/app/src/containers/App.js#L24).
- Main screens are `config`, `mode-Image`, `wifi`, `socket-test`, and `help`; only `config`, `mode-Image`, `socket-test`, `wifi`, and `help` are routed, while `mode-Light` and `mode-MIDI` appear in nav but have no implementation and fall into the 404 branch in [`src/containers/App.js`](/home/wizard/projects/asimov/MS4000/web/app/src/containers/App.js#L28).
- The app blocks initial render until `MS4.proto` is loaded at runtime in [`src/index.js`](/home/wizard/projects/asimov/MS4000/web/app/src/index.js#L41) and [`src/utils/protoBufLoader.js`](/home/wizard/projects/asimov/MS4000/web/app/src/utils/protoBufLoader.js#L6).

## Runtime Architecture

### Frontend Stack

- React 16.4, Redux 4, `redux-thunk`, `redux-undo`, `protobufjs`, `socket.io-client`, `upng-js`, `immutable`: see [`package.json`](/home/wizard/projects/asimov/MS4000/web/app/package.json).
- Middleware is thunk plus `redux-logger` in non-production in [`src/index.js`](/home/wizard/projects/asimov/MS4000/web/app/src/index.js#L21).
- Redux sagas exist but are disabled at boot; the imports and `sagaMiddleware.run(rootSaga)` are commented out in [`src/index.js`](/home/wizard/projects/asimov/MS4000/web/app/src/index.js#L14).

### Store Shape

The root reducer combines:

- `ms3000`
- `fileSystem`
- `sidebar`
- `sockets`
- `pixelEditor`
- `navigation`

Defined in [`src/reducers/index.js`](/home/wizard/projects/asimov/MS4000/web/app/src/reducers/index.js).

### Navigation Model

- Navigation is Redux state only, not URL-based, in [`src/reducers/navigation.js`](/home/wizard/projects/asimov/MS4000/web/app/src/reducers/navigation.js#L3).
- Default location is `config`.
- Nav entries are hard-coded in [`src/containers/Navigation.js`](/home/wizard/projects/asimov/MS4000/web/app/src/containers/Navigation.js#L14).

## Main Functional Areas

### 1. Config Editor

- `Config` renders an auto-generated UI from protobuf metadata via `AutoInterface` in [`src/containers/Config.js`](/home/wizard/projects/asimov/MS4000/web/app/src/containers/Config.js#L95).
- Changes update `state.ms3000.shifterState` through `CONFIG_UPDATE`.
- Optional fast-sync throttles uploads with a 500ms debounce in [`src/containers/Config.js`](/home/wizard/projects/asimov/MS4000/web/app/src/containers/Config.js#L21).
- Download flow:
  - `GET {host}/protobuf`
  - response is expected to be base64 text
  - decoded bytes are passed through `pb.MS4.decode`
  - result is converted to plain object and stored
  - implemented in [`src/actions/ms3000.js`](/home/wizard/projects/asimov/MS4000/web/app/src/actions/ms3000.js#L222)
- Upload flow:
  - current state is protobuf-encoded
  - bytes are converted to a string and base64 encoded
  - sent as `POST {host}/protobuf?myArg=<base64>`
  - implemented in [`src/actions/ms3000.js`](/home/wizard/projects/asimov/MS4000/web/app/src/actions/ms3000.js#L172)

### 2. Pixel Editor

- Main editor container: [`src/containers/PixelEditor.js`](/home/wizard/projects/asimov/MS4000/web/app/src/containers/PixelEditor.js#L95)
- Core state is undoable and stored in `state.pixelEditor.present` via `redux-undo` in [`src/reducers/pixelEditor.js`](/home/wizard/projects/asimov/MS4000/web/app/src/reducers/pixelEditor.js#L364).
- Features currently present:
  - draw, fill, erase, scroll, pick tools
  - palette and image-derived palette
  - frame list with add, duplicate, delete, reorder
  - PNG export
  - PNG or MagicBitmap import
  - upload current image to device
- Keyboard undo/redo is wired to `Ctrl/Cmd+Z` and `Ctrl/Cmd+Y` in [`src/containers/PixelEditor.js`](/home/wizard/projects/asimov/MS4000/web/app/src/containers/PixelEditor.js#L109).

### 3. Files Sidebar

- Device file browser is in [`src/containers/FilesSidebar.js`](/home/wizard/projects/asimov/MS4000/web/app/src/containers/FilesSidebar.js#L17).
- Refresh calls `GET {host}/json/files?dir=` in [`src/actions/filesystem.js`](/home/wizard/projects/asimov/MS4000/web/app/src/actions/filesystem.js#L33).
- Double-click or button download loads a MagicBitmap from the device into the pixel editor through [`src/actions/ms3000.js`](/home/wizard/projects/asimov/MS4000/web/app/src/actions/ms3000.js#L117).
- Sidebar visibility and selection are stored separately in `sidebar`.

### 4. Socket Test / WiFi

- `SocketCmdCenter` is a simple socket chat/test UI in [`src/containers/SocketCmdCenter.js`](/home/wizard/projects/asimov/MS4000/web/app/src/containers/SocketCmdCenter.js).
- `WiFiConfig` is mostly placeholder text and reuses socket props that are not actually rendered into a usable Wi-Fi editor in [`src/containers/WiFiConfig.js`](/home/wizard/projects/asimov/MS4000/web/app/src/containers/WiFiConfig.js).
- Saga/socket implementation exists in [`src/sagas/sockettest.js`](/home/wizard/projects/asimov/MS4000/web/app/src/sagas/sockettest.js#L6), but because saga startup is commented out, none of it currently runs.

## Data And Format Model

### Protobuf

- Runtime loads [`public/MS4.proto`](/home/wizard/projects/asimov/MS4000/web/app/public/MS4.proto) into `protobufjs`.
- `AutoInterface` walks protobuf fields and renders controls in field-number order in [`src/components/AutoInterface/index.js`](/home/wizard/projects/asimov/MS4000/web/app/src/components/AutoInterface/index.js#L35).
- `AutoControl` supports:
  - `string`
  - `int32`
  - `RGB`
  - `MIDI_OCTAVE`
  - `MIDI_SEQUENCE`
  - enums
  - nested message types
  - see [`src/components/AutoInterface/AutoControl.js`](/home/wizard/projects/asimov/MS4000/web/app/src/components/AutoInterface/AutoControl.js)

### Image Model

- Internal image abstraction is [`src/ms3000/Image.js`](/home/wizard/projects/asimov/MS4000/web/app/src/ms3000/Image.js).
- Frames are `immutable` `List`s of `{R,G,B}` pixels.
- PNG encode/decode uses `upng-js`.

### MagicBitmap

- Custom binary format implementation lives in [`src/ms3000/MagicBitmap.js`](/home/wizard/projects/asimov/MS4000/web/app/src/ms3000/MagicBitmap.js).
- Types supported in code: `bitmap`, `font`, `bitmap2`.
- The format implementation is incomplete:
  - `bitmap2` delay block is declared but not written/read end-to-end.
  - decode uses a hardcoded `delaysHACK` array of `2323` ms per frame in [`src/ms3000/MagicBitmap.js`](/home/wizard/projects/asimov/MS4000/web/app/src/ms3000/MagicBitmap.js#L248).
  - `fromMagicBitmap`, `toMagicBitmap`, `mirror` are unimplemented.

## External Integration Surface

Current device host default is `http://MS3000-7161.local` in [`src/reducers/ms3000.js`](/home/wizard/projects/asimov/MS4000/web/app/src/reducers/ms3000.js#L25).

Expected HTTP endpoints:

- `POST /upload`
  - multipart form field `uploadFile`
  - used for MagicBitmap upload
- `GET /download?file=<name>`
  - returns MagicBitmap bytes
- `GET /json/files?dir=`
  - returns JSON with `files`
- `GET /protobuf`
  - returns base64 protobuf text
- `POST /protobuf?myArg=<base64>`
  - accepts base64 protobuf payload

Expected socket endpoint:

- `http://localhost:8123` in [`src/sagas/sockettest.js`](/home/wizard/projects/asimov/MS4000/web/app/src/sagas/sockettest.js#L6)
- only relevant if saga startup is restored

## Current Condition By Area

### Working Or Mostly Working

- App boot, protobuf load gate, Redux store setup.
- Config screen rendering from protobuf definitions.
- Pixel editing state machine with undo/redo.
- PNG export/import.
- Production build completes.

### Partial / Fragile

- MagicBitmap animation timing.
- Files sidebar synchronization between real device state and optimistic local state.
- Navigation contains links to unimplemented views.
- Wi-Fi screen is not implemented beyond copy.
- Socket features are present in code but inactive in runtime.

### Broken

- Test suite fails immediately because [`src/App.test.js`](/home/wizard/projects/asimov/MS4000/web/app/src/App.test.js#L3) imports `./App`, but the actual app component lives under `containers/App.js`.
- `mode-Light` and `mode-MIDI` menu entries route to the fallback error view because they are not handled in [`src/containers/App.js`](/home/wizard/projects/asimov/MS4000/web/app/src/containers/App.js#L28).

## Highest-Risk Findings

### 1. Socket architecture is effectively dead code

- Sagas are disabled at startup in [`src/index.js`](/home/wizard/projects/asimov/MS4000/web/app/src/index.js#L14).
- `SocketCmdCenter` and `WiFiConfig` dispatch socket actions, but without saga middleware nothing connects to the server.
- Result: socket-related UI is misleading and cannot work end-to-end in the current app.

### 2. Filesystem reducer contains placeholder state and a bad remove implementation

- The reducer boots with fake file entries in [`src/reducers/fileSystem.js`](/home/wizard/projects/asimov/MS4000/web/app/src/reducers/fileSystem.js#L9), so the UI can look populated before any real fetch.
- Delete logic uses `files.slice().splice(...)`, which returns the removed items instead of the remaining array, in [`src/reducers/fileSystem.js`](/home/wizard/projects/asimov/MS4000/web/app/src/reducers/fileSystem.js#L37).
- It also skips removal of index `0` because the condition is `idx2 > 0`.

### 3. MagicBitmap timing is not trustworthy

- Upload creates `new MagicBitmap('bitmap', 24, width, height, frames, [999])` in [`src/actions/ms3000.js`](/home/wizard/projects/asimov/MS4000/web/app/src/actions/ms3000.js#L45).
- `MagicBitmap` requires delay-array length to equal frame count in [`src/ms3000/MagicBitmap.js`](/home/wizard/projects/asimov/MS4000/web/app/src/ms3000/MagicBitmap.js#L95).
- Any multi-frame upload will throw before request send.
- Downloaded/imported animation delays are replaced by a hardcoded `2323` ms value in [`src/ms3000/MagicBitmap.js`](/home/wizard/projects/asimov/MS4000/web/app/src/ms3000/MagicBitmap.js#L248).

### 4. Navigation advertises unimplemented screens

- `mode-Light` and `mode-MIDI` are present in [`src/containers/Navigation.js`](/home/wizard/projects/asimov/MS4000/web/app/src/containers/Navigation.js#L14).
- No matching route exists in [`src/containers/App.js`](/home/wizard/projects/asimov/MS4000/web/app/src/containers/App.js#L28).
- Result: user-visible dead ends.

### 5. Test coverage is functionally absent

- The only test is stale and broken in [`src/App.test.js`](/home/wizard/projects/asimov/MS4000/web/app/src/App.test.js#L1).
- There is no meaningful regression safety net for the editor, config serialization, or API integration.

### 6. Old React patterns and ref usage increase maintenance risk

- Extensive class components and string refs appear throughout, for example [`src/containers/Config.js`](/home/wizard/projects/asimov/MS4000/web/app/src/containers/Config.js#L35), [`src/containers/PixelEditor.js`](/home/wizard/projects/asimov/MS4000/web/app/src/containers/PixelEditor.js#L194), and [`src/components/AutoInterface/RGBControl.js`](/home/wizard/projects/asimov/MS4000/web/app/src/components/AutoInterface/RGBControl.js#L15).
- This is not immediately fatal, but it raises upgrade cost and makes behavior harder to verify.

## Notable Code Smells

- Unused import `legacyFetch` in [`src/actions/ms3000.js`](/home/wizard/projects/asimov/MS4000/web/app/src/actions/ms3000.js#L1).
- Reducer imports unused filesystem action types in [`src/reducers/ms3000.js`](/home/wizard/projects/asimov/MS4000/web/app/src/reducers/ms3000.js#L21).
- Duplicate JSX prop `className` in nav item markup in [`src/containers/Navigation.js`](/home/wizard/projects/asimov/MS4000/web/app/src/containers/Navigation.js#L49).
- Anchor links use `href="#"` in nav in [`src/containers/Navigation.js`](/home/wizard/projects/asimov/MS4000/web/app/src/containers/Navigation.js#L50).
- Multiple `throw "string"` patterns in image and MagicBitmap code.
- `PixelCanvas` has explicit unreachable branches and many warnings from stale code paths.

## Verification Snapshot

### Build

- `npm run build`: passes with warnings.
- Bundle output is large: main JS gzip size is about `549.99 KB`.

### Tests

- `CI=true npm test -- --watch=false`: fails.
- Failure reason: `Cannot find module './App' from 'App.test.js'`.

## Recommended Next Steps

1. Decide whether socket features are still in scope. If not, remove the dead saga/socket path and the placeholder Wi-Fi/chat UI. If yes, re-enable saga middleware and test server connectivity.
2. Fix the image transfer path first: pass real `framesDelays` into `MagicBitmap`, implement proper delay decoding, and correct filesystem optimistic update logic.
3. Remove or hide unimplemented nav entries so the app surface matches reality.
4. Replace placeholder filesystem seed data with empty state and real loading/error behavior.
5. Repair the test harness and add at least smoke coverage for app boot, protobuf load, config serialization, and pixel editor reducers.
6. Plan a modernization pass after stabilization: React upgrade, removal of string refs, and replacement of deprecated patterns.

## Bottom Line

The app is not abandoned, but it is in a transitional legacy state. The image editor and protobuf config editor still form a coherent core, while sockets, Wi-Fi management, tests, and parts of the file-format layer are incomplete or misleading. The fastest path to a trustworthy system is to narrow the supported surface, fix the image/config transport edges, and re-establish minimal automated coverage.
