# Vite Migration Risk Report

Date: 2026-04-09

## Purpose

This report lists the main problems that may arise while switching the app from Create React App to Vite for development, builds, preview, and testing. It also proposes mitigation strategies for each risk.

This report builds on:

- current-state analysis in [`docs/plan/system-now.md`](/home/wizard/projects/asimov/MS4000/web/app/docs/plan/system-now.md)
- target migration plan in [`docs/pla/system-target.md`](/home/wizard/projects/asimov/MS4000/web/app/docs/pla/system-target.md)

## Executive Summary

The migration is feasible, but it is not just a script swap. The main risks are:

- CRA-specific asset and env assumptions
- protobuf bootstrapping failures
- stale service worker behavior
- test runner breakage during the switch from CRA/Jest to Vitest
- hidden compatibility problems caused by the app’s age and older React patterns

The highest-risk technical edge is protobuf and public-asset loading. The highest-risk workflow edge is assuming Vite migration will automatically fix the broken test situation. It will not. The test layer needs deliberate repair.

## Risk Scale

- Likelihood: Low, Medium, High
- Impact: Low, Medium, High

## Risk Register

### 1. Protobuf File Stops Loading In Dev Or Build

Likelihood: High

Impact: High

Why this may happen:

- The app blocks initial render on protobuf load in [`src/index.js`](/home/wizard/projects/asimov/MS4000/web/app/src/index.js#L41).
- The loader currently uses `process.env.PUBLIC_URL + "MS4.proto"` in [`src/utils/protoBufLoader.js`](/home/wizard/projects/asimov/MS4000/web/app/src/utils/protoBufLoader.js#L10).
- That is CRA-specific and fragile.

Failure symptoms:

- blank app on startup
- rejected promise before `ReactDOM.render`
- config screen never appears
- console errors from `protobuf.load(...)`

Mitigation strategy:

- change the loader to use a Vite-safe public path
- preferred initial target: `/MS4.proto`
- verify under:
  - `vite dev`
  - `vite build`
  - `vite preview`
- add one test or smoke verification specifically for protobuf bootstrap

Recommended validation:

- open app in dev and confirm config screen renders
- preview built app and confirm same behavior

### 2. Static CSS And Favicon Assets Stop Loading

Likelihood: High

Impact: Medium

Why this may happen:

- HTML currently relies on `%PUBLIC_URL%` substitutions in [`public/index.html`](/home/wizard/projects/asimov/MS4000/web/app/public/index.html#L14).
- Vite does not use CRA’s HTML templating model.

Failure symptoms:

- layout breaks
- Pure CSS grid classes stop working
- missing favicon or manifest
- app renders without expected theme styling

Mitigation strategy:

- move all required HTML links into root Vite `index.html`
- use root-relative links:
  - `/pure-min.css`
  - `/pure-grid-min.css`
  - `/theme.css`
  - `/favicon.ico`
  - `/manifest.json`
- compare the rendered UI before and after migration

Recommended validation:

- open `config` and `mode-Image`
- verify grid layout and menus still render correctly

### 3. Service Worker Causes Stale Or Broken App Behavior

Likelihood: Medium

Impact: High

Why this may happen:

- legacy CRA service worker registration still exists in [`src/registerServiceWorker.js`](/home/wizard/projects/asimov/MS4000/web/app/src/registerServiceWorker.js#L21).
- `src/index.js` still calls it in [`src/index.js`](/home/wizard/projects/asimov/MS4000/web/app/src/index.js#L52).
- Old service worker state may continue affecting page loads after the migration.

Failure symptoms:

- app serves stale JS after deployment
- behavior differs between first and second refresh
- confusing caching bugs during verification

Mitigation strategy:

- remove service worker registration as part of the migration
- delete or archive the file
- explicitly instruct developers to unregister old service workers once after the switch
- verify in a clean browser profile or after unregistering the worker

Recommended validation:

- browser devtools:
  - check service workers
  - unregister legacy worker
  - hard reload

### 4. `process.env` Usage Breaks Under Vite

Likelihood: High

Impact: Medium

Why this may happen:

- current code uses CRA env patterns:
  - `process.env.NODE_ENV` in [`src/index.js`](/home/wizard/projects/asimov/MS4000/web/app/src/index.js#L27)
  - `process.env.PUBLIC_URL` in [`src/registerServiceWorker.js`](/home/wizard/projects/asimov/MS4000/web/app/src/registerServiceWorker.js#L24)
  - `process.env.PUBLIC_URL` in [`src/utils/protoBufLoader.js`](/home/wizard/projects/asimov/MS4000/web/app/src/utils/protoBufLoader.js#L10)

Failure symptoms:

- compile-time failures
- undefined env values at runtime
- wrong prod/dev behavior

Mitigation strategy:

- replace env access intentionally:
  - `process.env.NODE_ENV !== 'production'` -> `!import.meta.env.PROD`
  - `process.env.PUBLIC_URL` -> explicit public paths or Vite URL helpers
- run both dev and build verification after each env-related change

Recommended validation:

- confirm `redux-logger` only runs in dev
- confirm no runtime errors mention `process is not defined`

### 5. Dev Server Behavior Differs In Ways That Expose Hidden Issues

Likelihood: Medium

Impact: Medium

Why this may happen:

- CRA’s old webpack dev server and Vite differ in module resolution, HMR behavior, and asset serving.
- This codebase is old and has stale patterns that CRA may have tolerated.

Failure symptoms:

- imports fail unexpectedly
- hot reload behaves inconsistently
- dev-only blank screens or reload loops

Mitigation strategy:

- migrate in small steps
- first get cold-start rendering working
- then validate edit-refresh flow
- keep initial Vite config minimal rather than introducing aliases and custom transforms early

Recommended validation:

- edit a simple component and confirm fast refresh or full reload remains stable

### 6. Test Migration Fails Because The Existing Test Layer Is Already Broken

Likelihood: High

Impact: High

Why this may happen:

- the current test suite already fails:
  - [`src/App.test.js`](/home/wizard/projects/asimov/MS4000/web/app/src/App.test.js#L3) imports `./App`, which does not exist
- changing to Vitest does not solve bad test code by itself

Failure symptoms:

- `npm test` remains red after migration
- team assumes Vite is the problem when the issue predates it

Mitigation strategy:

- treat the test migration as a repair task, not a port-only task
- replace the broken smoke test with a correct app bootstrap test
- add a minimal test setup for DOM rendering if needed
- keep the first target small: one passing smoke test

Recommended validation:

- `npm test` passes after migration with at least one smoke test

### 7. React 16 Compatibility With Current Vite Tooling Needs Care

Likelihood: Medium

Impact: Medium

Why this may happen:

- the app uses React 16.4 in [`package.json`](/home/wizard/projects/asimov/MS4000/web/app/package.json#L19)
- Vite itself is fine with older React apps, but surrounding tooling defaults often assume more modern stacks

Failure symptoms:

- unexpected transform issues
- test environment mismatch
- docs/examples from modern Vite/React setups do not apply directly

Mitigation strategy:

- keep React version unchanged during migration
- use the standard React plugin without combining the migration with framework upgrades
- only modernize React after Vite is stable

Recommended validation:

- verify rendering, event handling, and class component behavior on `config` and `mode-Image`

### 8. Public Base Path Assumptions May Break Non-Root Deployments

Likelihood: Medium

Impact: Medium

Why this may happen:

- CRA previously masked some public path concerns through `%PUBLIC_URL%`
- Vite usually assumes root unless `base` is configured explicitly

Failure symptoms:

- app works locally but breaks when deployed under a subpath
- CSS or JS 404s in production
- `MS4.proto` fetch path incorrect in production

Mitigation strategy:

- decide early whether the app is root-hosted only
- if yes, use root-relative paths and document that assumption
- if no, configure Vite `base` deliberately and test under that base

Recommended validation:

- preview using the same path assumptions as production

### 9. Legacy `public/index.html` And Vite Root `index.html` Drift Or Conflict

Likelihood: Medium

Impact: Low

Why this may happen:

- the repo currently has CRA HTML in [`public/index.html`](/home/wizard/projects/asimov/MS4000/web/app/public/index.html)
- Vite expects root `index.html`
- leaving both around during transition can confuse maintainers

Failure symptoms:

- changes made to the wrong HTML file
- duplicated metadata maintenance
- stale docs

Mitigation strategy:

- keep both only during a short transition window
- after Vite is stable, delete CRA `public/index.html`
- document the ownership change clearly in README

### 10. Developers Continue Using Old Commands

Likelihood: Medium

Impact: Medium

Why this may happen:

- current muscle memory is `npm start` / `react-scripts`
- the repo already has mixed documentation quality

Failure symptoms:

- devs invoke removed scripts
- inconsistent local environments
- stale instructions in notes or CI jobs

Mitigation strategy:

- update `package.json` scripts decisively
- update README
- remove `react-scripts` once Vite is confirmed
- if needed, temporarily keep `start` mapped to `vite` to reduce friction

### 11. Preview And Production Behave Differently From Dev

Likelihood: Medium

Impact: High

Why this may happen:

- dev server file serving is more forgiving than built asset serving
- protobuf and static files are especially sensitive here

Failure symptoms:

- app works in `vite dev` but fails in `vite preview`
- production-only asset 404s
- config UI fails only after build

Mitigation strategy:

- treat `vite preview` as mandatory validation, not optional
- do not sign off on migration using dev-only verification
- include preview verification in the definition of done

Recommended validation:

- run:
  - `npm run dev`
  - `npm run build`
  - `npm run preview`
- verify both main screens in preview

### 12. Migration Scope Creep Delays Delivery

Likelihood: High

Impact: Medium

Why this may happen:

- the app has many unrelated problems:
  - broken tests
  - dormant sagas
  - incomplete MagicBitmap support
  - unimplemented screens
- it is easy to turn “switch to Vite” into a full modernization project

Failure symptoms:

- migration drags on
- many edits with unclear ownership
- regressions become hard to attribute

Mitigation strategy:

- keep the scope narrow:
  - toolchain replacement
  - asset/env fixes required for Vite
  - test harness repair only to establish a working baseline
- defer React upgrade and feature cleanup

## Dependency And Ecosystem Risks

### Package Lock Churn

Likelihood: Medium

Impact: Low

Risk:

- adding Vite and Vitest will significantly change `package-lock.json`

Mitigation strategy:

- isolate the migration in its own change set
- avoid mixing unrelated dependency updates

### Outdated Babel-Era Assumptions

Likelihood: Medium

Impact: Medium

Risk:

- the app was built around old Babel/CRA assumptions and may contain syntax or patterns that need explicit handling

Mitigation strategy:

- rely on standard Vite React setup first
- only add extra config when an actual issue appears

## Operational Risks

### CI Or Automation Still Calls Old Scripts

Likelihood: Unknown

Impact: Medium

Risk:

- if any external job uses `npm start`, `npm run build` with CRA expectations, or `react-scripts` directly, migration can break that automation

Mitigation strategy:

- search CI and deployment config before removing `react-scripts`
- preserve command names where sensible:
  - `build`
  - `test`
- only change semantics behind them

### Old Browser Cache Pollutes Verification

Likelihood: Medium

Impact: Medium

Risk:

- cached bundles and service workers can make the migration appear broken or successful incorrectly

Mitigation strategy:

- verify in private window or clean profile
- explicitly clear service workers and cache during migration testing

## Recommended Mitigation Program

### 1. Use A Staged Cutover

- stage 1: add Vite and get dev boot working
- stage 2: fix assets and protobuf loading
- stage 3: remove service worker
- stage 4: migrate tests
- stage 5: remove CRA

This reduces ambiguity when something breaks.

### 2. Use A Fixed Verification Checklist

For every major migration step, verify:

- app boots
- config screen renders
- image editor renders
- protobuf file loads
- static CSS loads
- dev build works
- production preview works
- test command works

### 3. Keep One Branch Or PR Focused Only On Tooling

- do not combine with React upgrade
- do not mix with feature work
- do not mix with broad cleanup of warnings

### 4. Remove The Service Worker Early

- this is the cleanest risk reducer in the migration
- it removes one large source of false negatives and stale behavior

### 5. Add A Minimal Smoke Test Before Finishing

- the repo currently has no reliable regression signal
- one passing smoke test is enough to anchor the migration

## Suggested Go/No-Go Gates

### Gate 1: Vite Dev Boot

Proceed only if:

- app starts under Vite
- config page renders
- no protobuf load failure

### Gate 2: Built Preview

Proceed only if:

- `vite build` succeeds
- `vite preview` behaves like dev for main screens

### Gate 3: Test Baseline

Proceed to remove CRA only if:

- Vitest is wired
- at least one smoke test passes

### Gate 4: CRA Removal

Proceed only if:

- no `process.env.PUBLIC_URL` remains
- no `react-scripts` dependency remains
- docs point to Vite commands only

## Bottom Line

The biggest problems likely to arise are not Vite itself, but the assumptions this app currently hides behind CRA: public asset templating, protobuf path loading, service worker registration, and a broken test harness. The safest mitigation strategy is a staged migration with explicit verification after each cut, early removal of the legacy service worker, and a deliberate reset of the test baseline.
