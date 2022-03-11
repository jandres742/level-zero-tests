$path=$args[0]
$name=$args[1]
$version=$args[2]
$src=$args[3]

$ErrorActionPreference = 'SilentlyContinue'
dt asset execute -- size `
  --root-url https://gfx-assets.fm.intel.com/artifactory `
  $path $name $version 2>&1 >$null
$rc = $?
$ErrorActionPreference = 'Stop'
if ($rc) {
    echo "asset already exists, skipping publish"
} else {
    dt asset publish `
      --root-url https://gfx-assets.fm.intel.com/artifactory `
      $path $name $version $src
}
