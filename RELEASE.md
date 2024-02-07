# Release step

- modify fibjs/include/version.h, change version string.
- run tools/changelog.js, generate changelog.
- update&translate docs.
- commit fibjs with "Release v#.##"
- merge dev into master
- tag fibjs with "v#.##"
- switch branch of fibjs to dev.
- modify fibjs/include/version.h, change version string to "dev".
- commit fibjs with "dev v#.##"
