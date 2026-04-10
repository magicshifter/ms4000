# System Target

Date: 2026-04-09

## Goal

Switch this app from `react-scripts` to Vite for all frontend workflows:

- local development
- production builds
- previewing production builds
- test execution

The target state should remove CRA as a runtime and tooling dependency. After migration, developers should use Vite-native commands only.

## Why Change

Current frontend tooling is anchored to `react-scripts` in [`package.json`](/home/wizard/projects/asimov/MS4000/web/app/package.json#L38). That creates several problems:

- old CRA 1.x toolchain and config model
- hidden webpack/dev-server behavior
- legacy service worker bootstrap in [`src/registerServiceWorker.js`](/home/wizard/projects/asimov/MS4000/web/app/src/registerServiceWorker.js)
- `%PUBLIC_URL%` HTML templating in [`public/index.html`](/home/wizard/projects/asimov/MS4000/web/app/public/index.html#L14)
- runtime asset resolution through `process.env.PUBLIC_URL` in [`src/utils/protoBufLoader.js`](/home/wizard/projects/asimov/MS4000/web/app/src/utils/protoBufLoader.js#L10)
- Jest setup tied to CRA scripts

Vite is the better fit for this codebase because it simplifies dev startup, makes asset handling explicit, and reduces dependence on obsolete build conventions.

## Target Architecture

### Tooling

- `vite` for dev server and production bundling
- `@vitejs/plugin-react` for React transform support
- `vitest` for test execution
- optional `jsdom` for component tests

### Package Scripts

Replace current scripts with a Vite-centered set:

- `dev`: `vite`
- `build`: `vite build`
- `preview`: `vite preview`
- `test`: `vitest run`
- `test:watch`: `vitest`
- keep `testServer`, `proto`, `loc` as needed

### App Entry

- Keep `src/index.js` as the app bootstrap initially to minimize migration risk.
- Move HTML shell ownership to a Vite root `index.html`.
- Use standard module entry loading from that root HTML.

### Assets

- Keep static assets in `public/` unless there is a reason to import them through JS.
- Reference them via root-relative paths in HTML, not `%PUBLIC_URL%`.
- Resolve `MS4.proto` via Vite-safe public path logic rather than `process.env.PUBLIC_URL`.

### Environment Model

- Replace CRA env assumptions with Vite env conventions:
  - `process.env.NODE_ENV` -> `import.meta.env.MODE` or `import.meta.env.PROD` / `DEV`
  - `process.env.PUBLIC_URL` -> root-relative asset paths or `new URL(..., import.meta.url)` when imported

### Service Worker

There are two acceptable targets:

1. Remove service worker support entirely for the first Vite cut.
2. Reintroduce it later with an explicit Vite-compatible solution.

Recommended: remove it in phase 1. The existing file is legacy CRA boilerplate and not a stable migration dependency.

## Scope

### In Scope

- replace `react-scripts start`
- replace `react-scripts build`
- replace CRA test runner
- replace CRA HTML template conventions
- remove CRA-only environment/path assumptions
- ensure protobuf file loading still works in dev and build
- document final commands for developers

### Out Of Scope

- React upgrade
- reducer/component refactors
- fixing all lint warnings
- redesigning navigation or feature behavior
- reviving socket sagas

Those can happen later, but they should not block the toolchain migration.

## Migration Plan

### Phase 1. Add Vite Without Changing App Behavior

1. Add dependencies:
   - `vite`
   - `@vitejs/plugin-react`
   - `vitest`
   - `jsdom`
2. Create `vite.config.js`.
3. Create root `index.html` for Vite.
4. Point the HTML entry at `src/index.js`.
5. Add Vite scripts to `package.json` without deleting old ones yet.

Acceptance criteria:

- `npm run dev` serves the app.
- `npm run build` via Vite produces a deployable bundle.
- `npm run preview` serves the built output locally.

### Phase 2. Remove CRA-Specific Runtime Assumptions

1. Replace `%PUBLIC_URL%` references in HTML:
   - [`public/index.html`](/home/wizard/projects/asimov/MS4000/web/app/public/index.html#L14)
2. Move needed HTML metadata and stylesheet links into Vite root `index.html`.
3. Replace protobuf load path logic in [`src/utils/protoBufLoader.js`](/home/wizard/projects/asimov/MS4000/web/app/src/utils/protoBufLoader.js#L10).
   - target should load `/MS4.proto` directly from the public root
   - if base-path support matters later, use Vite base config deliberately
4. Replace `process.env.NODE_ENV` usage in [`src/index.js`](/home/wizard/projects/asimov/MS4000/web/app/src/index.js#L27) and [`src/registerServiceWorker.js`](/home/wizard/projects/asimov/MS4000/web/app/src/registerServiceWorker.js#L22).

Acceptance criteria:

- no production/dev behavior depends on `process.env.PUBLIC_URL`
- app renders after protobuf load in both dev and built preview
- static CSS assets from `public/` still load correctly

### Phase 3. Remove Legacy Service Worker Coupling

1. Remove import and invocation of `registerServiceWorker` from [`src/index.js`](/home/wizard/projects/asimov/MS4000/web/app/src/index.js#L4).
2. Delete or archive [`src/registerServiceWorker.js`](/home/wizard/projects/asimov/MS4000/web/app/src/registerServiceWorker.js).
3. Remove any stale manifest/service-worker expectations that no longer apply.

Recommended target:

- no service worker in the initial Vite system

Rationale:

- lower migration risk
- fewer cache-debugging issues
- avoids carrying forward opaque CRA-generated offline behavior

Acceptance criteria:

- dev and production builds run without service worker code
- no stale cached-app behavior from old registration path

### Phase 4. Move Tests Off CRA

1. Replace CRA/Jest script usage with Vitest.
2. Repair the stale test import in [`src/App.test.js`](/home/wizard/projects/asimov/MS4000/web/app/src/App.test.js#L3).
3. Decide whether to keep the existing test filename pattern or rename to Vite/Vitest-friendly conventions.
4. Add a minimal test setup if React DOM rendering requires it.

Minimum target:

- one passing smoke test for app boot

Preferred target:

- smoke test for app bootstrap
- reducer tests for `pixelEditor`
- serialization/load test for protobuf bootstrap boundaries

Acceptance criteria:

- `npm test` runs under Vitest
- tests do not require `react-scripts`

### Phase 5. Remove CRA Completely

1. Delete `react-scripts` from dependencies.
2. Remove `eject` script.
3. Delete CRA-only HTML/template leftovers.
4. Update README and developer docs to Vite commands only.

Acceptance criteria:

- repo has no dependency on `react-scripts`
- all documented frontend workflows use Vite

## Concrete File Changes Expected

### New Files

- `vite.config.js`
- root `index.html`
- optional `vitest.config.js` if config is kept separate
- optional test setup file such as `src/test/setup.js`

### Files To Update

- [`package.json`](/home/wizard/projects/asimov/MS4000/web/app/package.json)
- [`src/index.js`](/home/wizard/projects/asimov/MS4000/web/app/src/index.js)
- [`src/utils/protoBufLoader.js`](/home/wizard/projects/asimov/MS4000/web/app/src/utils/protoBufLoader.js)
- [`src/App.test.js`](/home/wizard/projects/asimov/MS4000/web/app/src/App.test.js)

### Files Likely To Delete

- [`src/registerServiceWorker.js`](/home/wizard/projects/asimov/MS4000/web/app/src/registerServiceWorker.js)
- CRA-owned HTML template file after migration:
  - [`public/index.html`](/home/wizard/projects/asimov/MS4000/web/app/public/index.html)

## Key Technical Decisions

### Decision 1: Keep `public/` For Static Runtime Files

Recommended.

Reason:

- `MS4.proto`
- favicon
- manifest
- pure CSS assets
- theme CSS

These already behave like public static assets. Vite supports this cleanly.

### Decision 2: Remove Service Worker First

Recommended.

Reason:

- current service worker is legacy CRA boilerplate
- no evidence it is a core requirement
- reduces migration complexity sharply

### Decision 3: Do Not Combine Toolchain Migration With React Upgrade

Recommended.

Reason:

- changing bundler and framework version at once makes failures harder to isolate
- this app already has enough runtime complexity around protobuf and custom binary formats

### Decision 4: Use Root-Relative Public Paths Initially

Recommended.

Reason:

- simplest migration from current app assumptions
- avoids premature base-path complexity
- can be generalized later if deployment needs a subpath

## Risks And Mitigations

### Risk: Protobuf file stops loading

Cause:

- current code uses `process.env.PUBLIC_URL + "MS4.proto"` in [`src/utils/protoBufLoader.js`](/home/wizard/projects/asimov/MS4000/web/app/src/utils/protoBufLoader.js#L10), which is both CRA-specific and missing a slash.

Mitigation:

- explicitly load `/MS4.proto`
- verify in `vite dev` and `vite preview`

### Risk: Old HTML assets disappear

Cause:

- CRA template variables currently inject CSS links from `public/`

Mitigation:

- port every `<link>` from [`public/index.html`](/home/wizard/projects/asimov/MS4000/web/app/public/index.html#L14) into Vite root `index.html`
- verify layout visually

### Risk: Test migration stalls because test setup is already broken

Cause:

- current test suite already fails due to bad import

Mitigation:

- treat test migration as a clean reset
- first establish one passing smoke test under Vitest

### Risk: Hidden CRA behaviors were masking path issues

Cause:

- current tooling may tolerate patterns Vite will not

Mitigation:

- do migration in phases
- verify each phase with `dev`, `build`, and `preview`

## Definition Of Done

The migration is complete when all of the following are true:

- `npm run dev` starts the app with Vite
- `npm run build` builds with Vite
- `npm run preview` serves the built app
- `npm test` runs with Vitest
- the app renders and loads protobuf definitions in both dev and preview
- image editor screen still works
- config screen still works
- no `react-scripts` dependency remains
- no CRA-only `process.env.PUBLIC_URL` or `%PUBLIC_URL%` usage remains

## Suggested Execution Order

1. Introduce Vite config and root HTML.
2. Make the current app boot under Vite without feature changes.
3. Fix public asset and protobuf loading.
4. remove service worker coupling.
5. migrate tests to Vitest.
6. remove `react-scripts`.
7. update docs.

## Bottom Line

The clean target is a Vite-native app with explicit public asset handling, no CRA service worker boilerplate, and Vitest-based tests. The migration should be done as a tooling replacement, not as a broader modernization project. That keeps the blast radius small and gives the team a stable baseline before any React or architecture upgrades.
