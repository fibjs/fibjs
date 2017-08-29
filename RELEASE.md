# Release step

* vender
  - merge dev into master
  - branch #.##.x

* fibjs
  - modify fibjs/include/version.h, change version string.
  - run tools/changelog.js, generate changelog.
  - change branch of vender to #.##.x in .gitmodules.
  - commit fibjs with "Release v#.##"
  - merge dev into master
  - branch #.##.x
  - tag fibjs with "v#.##"

* switch branch of vender to dev.
* fibjs
  - switch branch of fibjs to dev.
  - merge master into dev.
  - modify fibjs/include/version.h, change version string to "dev".
  - switch branch of vender to "dev" in .gitmodules.
  - commit fibjs with "dev v#.##"
