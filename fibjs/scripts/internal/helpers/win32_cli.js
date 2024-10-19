module.exports = function (sh, cli) {
    const posix_cli = cli.replace(/\\/g, '/');

    const sh_script = `#!/bin/sh
basedir=$(dirname "$(echo "$0" | sed -e 's,\\\\,/,g')")

case \`uname\` in
    *CYGWIN*|*MINGW*|*MSYS*) basedir=\`cygpath -w "$basedir"\`;;
esac

if [ -x "$basedir/${sh}" ]; then
  exec "$basedir/${sh}"  "$basedir/${posix_cli}" "$@"
else 
  exec ${sh}  "$basedir/${posix_cli}" "$@"
fi
`;

    const cmd_script = `@ECHO off
GOTO start
:find_dp0
SET dp0=%~dp0
EXIT /b
:start
SETLOCAL
CALL :find_dp0

IF EXIST "%dp0%\\${sh}.exe" (
  SET "_prog=%dp0%\\${sh}.exe"
) ELSE (
  SET "_prog=${sh}"
  SET PATHEXT=%PATHEXT:;.JS;=;%
)

endLocal & goto #_undefined_# 2>NUL || title %COMSPEC% & "%_prog%"  "%dp0%\\${cli}" %*
`

    const ps1_script = `#!/usr/bin/env pwsh
$basedir=Split-Path $MyInvocation.MyCommand.Definition -Parent

$exe=""
if ($PSVersionTable.PSVersion -lt "6.0" -or $IsWindows) {
  $exe=".exe"
}
$ret=0
if (Test-Path "$basedir/${sh}$exe") {
  # Support pipeline input
  if ($MyInvocation.ExpectingInput) {
    $input | & "$basedir/${sh}$exe"  "$basedir/${posix_cli}" $args
  } else {
    & "$basedir/${sh}$exe"  "$basedir/${posix_cli}" $args
  }
  $ret=$LASTEXITCODE
} else {
  # Support pipeline input
  if ($MyInvocation.ExpectingInput) {
    $input | & "${sh}$exe"  "$basedir/${posix_cli}" $args
  } else {
    & "${sh}$exe"  "$basedir/${posix_cli}" $args
  }
  $ret=$LASTEXITCODE
}
exit $ret
`

    return {
        sh: sh_script,
        cmd: cmd_script,
        ps1: ps1_script
    }

}
