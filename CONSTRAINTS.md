# Constraints

These rules apply to every commit in this repository. Reviews must enforce them.

## Git workflow

- `main` is protected: **never commit directly to it**.
- **Every feature, test, or fix gets its own branch** created from an up-to-date
  `main`, then merged with a merge commit. Example:
  - `feat/<name>` for app features,
  - `test/<name>` for new or updated tests,
  - `fix/<name>`, `docs/<name>`, `chore/<name>` for the rest.
- Commit messages use Conventional Commits (`feat:`, `test:`, `fix:`, `docs:`,
  `chore:`). See [CONTRIBUTING.md](CONTRIBUTING.md) for the full workflow.
- Tests must pass on a branch before it is merged into `main`.

## Merge approval

- **When changes are pushed to GitHub and the user has not explicitly said
  "merge", never merge to `main` automatically.**
- Instead, push the work on its own branch and leave it for the user to review
  and approve the merge on GitHub.
- If the user asks the agent to merge on their behalf, that explicit request
  is approval enough - the agent may then merge and push `main`.

## Code size

- Every source file must stay within **100-120 lines maximum**.
- If a file grows past the limit, split it (e.g. a separate `ui_dialogs.py`
  for dialog components, or a dedicated `storage.py` for persistence).

## Functions

- Keep functions small, focused, and single-purpose. **No monolithic functions**
  (a function must not load data, mutate the store, and render a UI at once).
- Prefer several small helpers over one large function.
- A function that becomes hard to read should be split into named helpers.

## Comments

- Comment *why*, not what - the code should already show what it does.
- Every public function, class, and dataclass gets a short docstring.
- Keep comments brief and to the point.
- **No `TODO:` markers or unfinished code may be committed.**

## Simplicity

- Keep things simple and direct. Avoid clever one-liners and unnecessary abstraction.
- Prefer plain data structures over over-engineered generic layers.
- No dead code: remove unused imports, fields, and methods before committing.

## Tests

- Tests are required for **important features**: task CRUD, dependency
  blocking, sorting, due-date validation, persistence, and the UI's key
  interactions. Not every function needs a test - small glue helpers and thin
  UI wrappers do not.
- Pure logic lives in testable modules (`models.py`, `storage.py`), separate
  from the NiceGUI page code.
- Run `pytest` and all tests must pass before merging.

## Maintainability & scalability

- Code must be maintainable, scalable, and optimal where it matters:
  - no accidental `O(n^2)` scans of the task list in hot paths,
  - search/filter logic pushes down into `models.py` helpers, not UI glue,
  - deterministic, side-effect-free helper functions where possible.
- Build cleanly with `ruff check` and `mypy` - zero errors.

## Style

- Python 3.10+, PEP 8 formatting (ruff defaults), line length 120.
- `snake_case` for functions and variables, `UPPER_SNAKE` for constants,
  `PascalCase` for classes.
- **Type hints are mandatory** on all public function signatures (see
  `task_manager/models.py`).
- Order helpers top-down: private helpers above the public items that use them.