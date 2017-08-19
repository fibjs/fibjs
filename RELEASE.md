# Release step

* vender
  - merge dev into master
  - branch #.##.x

* fibjs
  - modify fibjs/include/version.h, change version string.
  - run tools/changelog.js, generate changelog.
  - change branch of vender to #.##.x in .gitmodules.

* commit fibjs with "Release v#.##"
* tag fibjs with "v#.##"

* switch branch of vender to dev.
* switch branch of fibjs to dev.
* fibjs
  - modify fibjs/include/version.h, change version string to "dev".
  - switch branch of vender to "dev" in .gitmodules.
* commit fibjs with "dev v#.##"
