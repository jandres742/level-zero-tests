# Set the GITHASH tag from the tests repo
GITHASH=$3
# Checkout dave-compute-library repo to master
if [ -d "../../dave-compute-library" ]; then
    rm -rf ../../dave-compute-library
fi
git clone https://$2@github.com/nrspruit/drivers.gpu.validation.dave-compute-library.git ../../dave-compute-library
pushd ../../dave-compute-library
# create a L0 test item branch
git branch L0_Test_Item_UPDATE_$GITHASH
git checkout L0_Test_Item_UPDATE_$GITHASH
git remote add parent https://$2@github.com/intel-innersource/drivers.gpu.validation.dave-compute-library.git
git fetch parent
git rebase parent/master
popd

# Build the L0 Loader and NULL Driver
if [ -d "../../level_zero_loader" ]; then
    rm -rf ../../level_zero_loader
fi
git clone https://$2@github.com/intel-innersource/libraries.compute.oneapi.level-zero.loader.git ../../level_zero_loader
apt-get install build-essential cmake libpng16-16 -y
pushd ../../level_zero_loader
mkdir build
cd build
cmake ../
make -j `nproc`
popd
# Run the L0 test item generator
pwd
export ZE_ENABLE_NULL_DRIVER=1
python3 ci_generate_json_files.py --binary_dir $1 --level_zero_lib_dir ../../level_zero_loader/build/lib --dave_test_item_dir ../../dave-compute-library/library/test_items/level0/
RESULT=$?
if [ $RESULT -eq 0 ]; then
    # Create the commit and push the branch
    pushd ../../dave-compute-library
    git add -A
    git commit -a -m "L0 Test Item Update $GITHASH" -s
    RESULT=$?
    if [ $RESULT -eq 0 ]; then
        git push origin L0_Test_Item_UPDATE_$GITHASH -f
    fi
    echo Generate Json Successfully created
else
    echo Generate Json Failed
fi
