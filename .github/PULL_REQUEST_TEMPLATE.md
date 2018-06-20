Please fill in this template.

- [ ] Use a meaningful title for the pull request with prefix '(module|object|source_directory) [bugfix|feat|refactor|]', Such as `db, bugfix: ...`, `tools, feat: ...`
- [ ] Test the change in your own code. (Compile and run.)
- [ ] Add or edit tests to reflect the change. (Run with `fibjs test`.)
- [ ] Ensure commits history clean, merge master/dev and clean meaningless commits by `git rebase`.

Select type of change you made and delete the others:

If adding/changing one existing module or internal Object:
- [ ] Add/Change the corresponding `*.idl` file
- [ ] Run `fibjs ./tools/idlc.js` to auto-generate `*.h`, **never change it**

If removing one existing module or internal Object:
- [ ] Remove corresponding `*.h`, `*.idl`, `*.cpp`
- [ ] Remove all reference to it.

If adding internal js scripts:
- [ ] Add it to `fibjs/scripts`
- [ ] Run `fibjs ./tools/gen_script.js`
