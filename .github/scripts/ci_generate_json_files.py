#! /usr/bin/env python3

import sys
sys.path.append('../../scripts/')

import argparse
from argparse import RawTextHelpFormatter
import os
from typing import Dict,List,TextIO,Iterable
import subprocess
import json
import glob
import re
import level_zero_report_utils

allow_setting_basic_features = 0

def IsListableDirPath(path: str):
    try:
        os.listdir(path)
    except Exception as e:
        raise argparse.ArgumentTypeError(path + ' is not a listable directory: ' + str(e))
    return os.path.abspath(path)

def IsWritableDirPath(path: str):
    if not os.access(path, os.W_OK):
        raise argparse.ArgumentTypeError(path + ' is not writable')
    if not os.path.isdir(path):
        raise argparse.ArgumentTypeError(path + ' is not a directory')
    return os.path.abspath(path)

SmokeTestList = ["L0_CTS_SamplerCreationCombinations_zeDeviceCreateSamplerTests_GivenSamplerDescriptorWhenCreatingSamplerThenNotNullSamplerIsReturned",
                 "L0_CTS_SynchronousAndAsynchronousCommandQueueTests_zeCommandQueueSynchronousTests_GivenModeWhenCreatingCommandQueueThenSynchronousOrAsynchronousBehaviorIsConfirmed",
                 "L0_CTS_TestAllInputPermuations_zeCommandQueueCreateTests_GivenValidDescriptorWhenCreatingCommandQueueThenSuccessIsReturned",
                 "L0_CTS_TestIncreasingNumberCommandListsWithCommandQueueFence_zeCommandQueueExecuteCommandListTestsFence_GivenFenceSynchronizationWhenExecutingCommandListsThenSuccessIsReturned",
                 "L0_CTS_TestIncreasingNumberCommandListsWithSynchronize_zeCommandQueueExecuteCommandListTestsSynchronize_GivenCommandQueueSynchronizationWhenExecutingCommandListsThenSuccessIsReturned",
                 "L0_CTS_zeCommandListAppendMemoryCopyWithDataVerificationTests_GivenHostMemoryDeviceMemoryAndSizeWhenAppendingMemoryCopyThenSuccessIsReturnedAndCopyIsCorrect",
                 "L0_CTS_zeCommandQueueDestroyTests_GivenValidDeviceAndNonNullCommandQueueWhenDestroyingCommandQueueThenSuccessIsReturned",
                 "L0_CTS_zeDriverGetApiVersionTests_GivenValidDriverWhenRetrievingApiVersionThenValidApiVersionIsReturned",
                 "L0_CTS_zeDriverGetDriverVersionTests_GivenZeroVersionWhenGettingDriverVersionThenNonZeroVersionIsReturned",
                 "L0_CTS_zeInitTests_GivenGPUFlagWhenInitializingGPUDriverThenSuccessIsReturned",
                 "L0_CTS_zeInitTests_GivenNoneFlagWhenInitializingDriverThenSuccessIsReturned",
                 "L0_CTS_zeSamplerTests_GivenSamplerWhenPassingAsFunctionArgumentThenSuccessIsReturned",
                 "L0_CTS_zeCommandListAppendMemoryCopyWithDataVerificationTests_GivenDeviceMemoryToDeviceMemoryAndSizeWhenAppendingMemoryCopyThenSuccessIsReturnedAndCopyIsCorrect"]


def create_test_item_json(dave_library_path: str, test_name: str, test_filter: str, test_feature: str, test_feature_tag: str):
    if (test_feature.find("Images") != -1 ) or \
       (test_feature.find("Image Samplers") != -1) or \
       (test_feature.find("Samplers") != -1) or \
       (test_feature == "Kernels" and test_name.find("Image") != -1) or \
       (test_feature == "Kernels" and test_name.find("ManyArgs") != -1) or \
       (test_feature == "Command Lists" and test_name.find("VariousCommands") != -1) or \
       (test_name.find("Image") != -1) or \
       (test_name.find("Sampler") != -1):
        data = {
            "name": test_name,
            "namespace": "level_zero_conformance_tests_itemized",
            "description": "null",
            "owner": "nrspruit",
            "plugin": "test_compute",
            "arguments": "levelzero_conformance_tests " + test_filter,
            "assets": [
                "gfx-compute-assets/level-zero-tests-legacyformat-conformance"
            ],
            "metadata": {
                "required_capabilities": {
                    "ze_device_image_properties_t": {
                        "supported": "true"
                    }
                }
            },
            "attributes": {
                "Component": "Level Zero",
                "Feature": test_feature,
                "Test type": "Functional",
                "Timeout": "600",
                test_feature_tag: "true"
            }
        }
    elif test_feature_tag == "gta.planning.attributes.tags.LEVEL_ZERO_NEGATIVE_TESTS":
        data = {
            "name": test_name,
            "namespace": "level_zero_conformance_tests_itemized",
            "description": "null",
            "owner": "nrspruit",
            "plugin": "test_compute",
            "arguments": "levelzero_conformance_tests " + test_filter,
            "assets": [
                "gfx-compute-assets/level-zero-tests-legacyformat-negative"
            ],
            "attributes": {
                "Component": "Level Zero",
                "Feature": test_feature,
                "Test type": "Functional",
                "Timeout": "600",
                test_feature_tag: "true"
            }
        }
    elif test_feature_tag == "gta.planning.attributes.tags.LEVEL_ZERO_STRESS_TESTS":
        data = {
            "name": test_name,
            "namespace": "level_zero_conformance_tests_itemized",
            "description": "null",
            "owner": "nrspruit",
            "plugin": "test_compute",
            "arguments": "levelzero_conformance_tests " + test_filter,
            "assets": [
                "gfx-compute-assets/level-zero-tests-legacyformat-stress"
            ],
            "attributes": {
                "Component": "Level Zero",
                "Feature": test_feature,
                "Test type": "Functional",
                "Timeout": "600",
                test_feature_tag: "true"
            }
        }
    else:
        data = {
            "name": test_name,
            "namespace": "level_zero_conformance_tests_itemized",
            "description": "null",
            "owner": "nrspruit",
            "plugin": "test_compute",
            "arguments": "levelzero_conformance_tests " + test_filter,
            "assets": [
                "gfx-compute-assets/level-zero-tests-legacyformat-conformance"
            ],
            "attributes": {
                "Component": "Level Zero",
                "Feature": test_feature,
                "Test type": "Functional",
                "Timeout": "600",
                test_feature_tag: "true"
            }
        }
    json_name = dave_library_path + "/" + test_name + ".json"
    # If the json file already exists, then avoid overwrite
    # NOTE: this might change depending on how the automation is decided

    if test_name in SmokeTestList:
        smoke_tag = {"gta.planning.attributes.tags.LEVEL_ZERO_SMOKE": "true"}
        smoke_timeout = {"Timeout": "60"}
        data["attributes"].update(smoke_tag)
        data["attributes"].update(smoke_timeout)

    if not (os.path.exists(json_name)):
        print("test case " + test_name + " is new\n")
        if test_feature_tag == "gta.planning.attributes.tags.LEVEL_ZERO_BASIC_FEATURES" and not allow_setting_basic_features:
            advanced_tag = {"gta.planning.attributes.tags.LEVEL_ZERO_ADVANCED_FEATURES": "true"}
            basic_tag = {"gta.planning.attributes.tags.LEVEL_ZERO_BASIC_FEATURES": "false"}
            data["attributes"].update(advanced_tag)
            data["attributes"].update(basic_tag)
        with open(json_name, "w") as write_file:
            json.dump(data, write_file, indent=4)
    elif (os.path.exists(json_name)):
        with open(json_name, "r") as read_file:
            json_file_data = read_file.read()
            current_data = json.loads(json_file_data)
            for key in data:
                if key in current_data.keys():
                    if current_data[key] != data[key]:
                        if test_feature_tag == "gta.planning.attributes.tags.LEVEL_ZERO_BASIC_FEATURES" and not allow_setting_basic_features:
                            return
                        if key == "name" and current_data[key] != data[key]:
                            print("ERROR: test case " + test_name + " is being renamed, please revert rename.\n")
                            exit(-1)
                        if key == "attributes":
                            return
                        else:
                            current_data[key] = data[key]
                else:
                    current_data[key] = data[key]
        with open(json_name, "w") as write_file:
            json.dump(current_data, write_file, indent=4)


def assign_gta_test_feature_tag(test_feature: str, test_name: str, test_section: str,):
    test_feature_tag = level_zero_report_utils.assign_test_feature_tag(test_feature, test_name, test_section)
    if test_feature_tag == "basic":
        test_feature_tag = "gta.planning.attributes.tags.LEVEL_ZERO_BASIC_FEATURES"
    elif test_feature_tag == "advanced":
        test_feature_tag = "gta.planning.attributes.tags.LEVEL_ZERO_ADVANCED_FEATURES"
    elif test_feature_tag == "discrete":
        test_feature_tag = "gta.planning.attributes.tags.LEVEL_ZERO_DISCRETE_GPU_FEATURES"
    elif test_feature_tag == "tools":
        test_feature_tag = "gta.planning.attributes.tags.LEVEL_ZERO_TOOLS_FEATURES"
    elif test_feature_tag == "negative":
        test_feature_tag = "gta.planning.attributes.tags.LEVEL_ZERO_NEGATIVE_TESTS"
    elif test_feature_tag == "stress":
        test_feature_tag = "gta.planning.attributes.tags.LEVEL_ZERO_STRESS_TESTS"
    else:
        test_feature_tag = "gta.planning.attributes.tags.LEVEL_ZERO_ADVANCED_FEATURES"
    return test_feature_tag

def create_gta_test_item(suite_name: str, test_binary: str, line: str, dave_dir: str):
        test_section = "None"
        updated_suite_name = suite_name.split('/')  # e.g., 'GivenXWhenYThenZ'
        test_suite_name = updated_suite_name[0]
        if (len(updated_suite_name) > 1):
            test_suite_name += "_" + updated_suite_name[1]
        parameterized_case_name = line.split()[0]  # e.g., 'GivenXWhenYThenZ/1  # GetParam() = (0)' or just 'GivenXWhenYThenZ' if not parameterized
        case_name = parameterized_case_name.split('/')[0]  # e.g., 'GivenXWhenYThenZ'
        if (test_binary.find("_errors") != -1):
            test_name = "L0_NEG_" + test_suite_name + "_" + case_name
            test_section = "Negative"
        elif (re.search('stress', test_suite_name, re.IGNORECASE) or
             (re.search('stress', case_name, re.IGNORECASE))):
            test_name = "L0_STRESS_" + test_suite_name + "_" + case_name
        else:
            test_name = "L0_CTS_" + test_suite_name + "_" + case_name
        test_filter = "\"" + test_binary + " --gtest_filter=*" + case_name + "*" + "\""
        if test_name.find("DISABLED") != -1:
            return
        test_feature, test_section_by_feature = level_zero_report_utils.assign_test_feature(test_binary, test_name)
        if (test_section == "None"):
            test_section= test_section_by_feature
        test_feature_tag = assign_gta_test_feature_tag(test_feature, test_name, test_section)
        create_test_item_json(dave_dir, test_name, test_filter, test_feature, test_feature_tag)

def generate_test_items_for_binary(dave_dir: str, binary_dir: str, level_zero_lib_dir: str, test_binary: str):
    test_binary_path = os.path.join(binary_dir, test_binary)
    config=dict(os.environ, LD_LIBRARY_PATH=level_zero_lib_dir)
    if (test_binary.find("test") != -1):
        popen = subprocess.Popen([test_binary_path, '--gtest_list_tests', '--logging-level=error'], env=config, stdout=subprocess.PIPE)
        output = popen.stdout.read()
        output = output.decode('utf-8').splitlines()

        # parameterized cases are reduced to a single case for all parameterizations
        current_suite = None
        for line in output:
            if line[0] != ' ':  # test suite
                current_suite = line.split('.')[0]
            else:  # test case
                create_gta_test_item(current_suite, test_binary, line, dave_dir)
    else:
        print("ERROR: test case not gtest\n")

def generate_gta_test_items_from_binaries(dave_test_item_dir: str, binary_dir: str, level_zero_lib_dir: str):

    test_binaries = [
      filename for filename in os.listdir(binary_dir)
      if os.access(os.path.join(binary_dir, filename), os.X_OK) and
      os.path.isfile(os.path.join(binary_dir, filename))]

    for binary in test_binaries:
        print("processing binary " + binary + "\n")
        generate_test_items_for_binary(dave_test_item_dir, binary_dir, level_zero_lib_dir, binary)
        print("processed " + binary + "\n")


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='''Standalone JSON file generator for GTA Test Items for Level Zero CTS''',
    epilog="""example:\n
        python3 ci_generate_json_files.py \n
            --binary_dir /build/out/conformance_tests/ \n
            --level_zero_lib_dir /level_zero_loader/build/bin/ \n
            --dave_test_item_dir /dave-compute-library/library/test_items/level0/""", formatter_class=RawTextHelpFormatter)
    parser.add_argument('--binary_dir', type = IsListableDirPath, help = 'Directory containing gtest binaries and SPVs.')
    parser.add_argument('--level_zero_lib_dir', type = IsListableDirPath, help = 'Directory containing libze_loader.so.')
    parser.add_argument('--dave_test_item_dir', type = IsWritableDirPath, help = 'Directory to write GTA test items for dave')
    parser.add_argument('--update_standard_tests', type = bool, help = 'Allow Updating standardized tests for all platforms')
    parser.add_argument('--overwrite_existing_files', type = bool, help = 'Allow Updating existing tests')
    args = parser.parse_args()
    allow_setting_basic_features = args.update_standard_tests

    if args.overwrite_existing_files:
        print('Removing old JSON files')
        value = args.dave_test_item_dir+'/*.json'
        print(value)
        json_files = glob.glob(value)
        for f in json_files:
            try:
                os.remove(f)
            except OSError as e:
                print("Error: %s : %s" % (f, e.strerror))
        print('Removed old JSON files')
    print('Determining test cases to generate json files')
    generate_gta_test_items_from_binaries(
      dave_test_item_dir = args.dave_test_item_dir,
      binary_dir = args.binary_dir,
      level_zero_lib_dir = args.level_zero_lib_dir)
    print('Successfully Generated JSON files')
