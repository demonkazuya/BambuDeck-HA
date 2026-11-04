import json
import subprocess
import os
import hashlib
import shutil
from pathlib import Path

Import("env")

env = DefaultEnvironment()


def calculate_md5_and_size(file_path):
    # Create an MD5 hash object
    md5_hash = hashlib.md5()

    try:
        # Open the file in binary read mode
        with open(file_path, "rb") as file:
            # Read the file in chunks
            chunk_size = 4096  # You can adjust the chunk size as needed
            file_size = 0

            while True:
                chunk = file.read(chunk_size)
                if not chunk:
                    break
                md5_hash.update(chunk)
                file_size += len(chunk)

        # Get the hexadecimal representation of the MD5 hash
        md5_hex = md5_hash.hexdigest()

        return md5_hex, file_size

    except FileNotFoundError:
        print(f"File not found: {file_path}")
        return None, None


def delete_bin_files(directory):
    try:
        for filename in os.listdir(directory):
            if filename.endswith(".bin"):
                file_path = os.path.join(directory, filename)
                os.remove(file_path)
                print(f"Deleted: {file_path}")
        print("All .bin files deleted successfully.")
    except Exception as e:
        print(f"An error occurred: {str(e)}")


def post_build_increment_semver(json_file, bump_type="patch"):
    # Load the JSON from the file
    with open(json_file, 'r') as f:
        data = json.load(f)

    # Get the current version from the JSON
    version = data.get("version", "0.0.1").split(".")

    # Increment the version based on the bump type (major, minor, or patch)
    if bump_type == "major":
        version[0] = str(int(version[0]) + 1)
        version[1] = "0"
        version[2] = "0"
    elif bump_type == "minor":
        version[1] = str(int(version[1]) + 1)
        version[2] = "0"
    elif bump_type == "patch":
        version[2] = str(int(version[2]) + 1)
    else:
        raise ValueError("Bump type must be 'major', 'minor', or 'patch'.")

    # Update the version in the JSON
    data["version"] = ".".join(version)

    # Save the updated JSON to the file
    with open(json_file, 'w') as f:
        json.dump(data, f, indent=2)


def post_build_create_ota_json(version_value):

    md5, size = calculate_md5_and_size(
        f"dist/ota/bambudeck.{version_value}.bin")
    ota = {
        "version": version_value,
        "url": f"https://raw.githubusercontent.com/BambuDeck/ota/bambudeck.{version_value}.bin",
        "md5": md5,
    }

    # Serialize the webusb_manifest as a JSON string
    ota_serialized = json.dumps(ota, indent=2)

    # If you want to save it to a file, you can do:
    with open("dist/ota/ota.json", "w") as ota_file:
        ota_file.write(ota_serialized)


def post_build_manifest(version_value):

    webusb_manifest_fw_path = f"bambudeck.web.{version_value}.bin"

    # Create a new JSON object with the updated version and path values
    webusb_manifest = {
        "name": "bambudeck",
        "version": version_value,
        "builds": [
            {
                "chipFamily": "ESP32",
                "parts": [{"path": webusb_manifest_fw_path, "offset": 0}]
            }
        ]
    }

    # Serialize the webusb_manifest as a JSON string
    webusb_manifest_serialized = json.dumps(webusb_manifest, indent=2)

    # If you want to save it to a file, you can do:
    with open("dist/webusb/webusb.manifest.json", "w") as webusb_manifest_file:
        webusb_manifest_file.write(webusb_manifest_serialized)


def post_build_action(source, target, env):
    with open("version.json", "r") as version_file:
        version_data = json.load(version_file)
        version_value = version_data.get("version", "1.0.0")

    dist_dir = Path("dist")
    dist_dir.mkdir(exist_ok=True)

    # Copy firmware.bin to dist/
    fw_bin_source = ".pio/build/esp32dev/firmware.bin"
    fw_bin_target = f"dist/bambudeck.{version_value}.bin"
    shutil.copy(fw_bin_source, fw_bin_target)
    shutil.copy(fw_bin_source, "dist/bambudeck.bin")

    # Generate merged factory binary (bootloader + partitions + firmware)
    factory_bin = f"dist/bambudeck.factory.{version_value}.bin"
    platform = env.PioPlatform()
    esptool_pkg = platform.get_package_dir("tool-esptoolpy")
    esptool_path = os.path.join(esptool_pkg, "esptool.py") if esptool_pkg else "esptool.py"

    esptool_cmd = [
        env.subst("$PYTHONEXE"),
        esptool_path,
        '--chip', 'ESP32',
        'merge_bin',
        '-o', factory_bin,
        '--flash_mode', 'dio',
        '--flash_size', '4MB',
        '0x1000', '.pio/build/esp32dev/bootloader.bin',
        '0x8000', '.pio/build/esp32dev/partitions.bin',
        '0x10000', '.pio/build/esp32dev/firmware.bin'
    ]
    subprocess.run(esptool_cmd, check=True)
    shutil.copy(factory_bin, "dist/bambudeck.factory.bin")

    # Generate ota.json pointing to GitHub Release asset
    md5, size = calculate_md5_and_size(fw_bin_target)
    ota_data = {
        "version": version_value,
        "url": f"https://github.com/demonkazuya/BambuDeck-HA/releases/download/v{version_value}/bambudeck.bin",
        "md5": md5,
        "size": size
    }
    with open("ota.json", "w") as ota_file:
        json.dump(ota_data, ota_file, indent=2)
    with open("dist/ota.json", "w") as ota_file:
        json.dump(ota_data, ota_file, indent=2)

    print(f"BAMBUDECK POSTBUILD: Binaries and ota.json successfully created in dist/")


env.AddPostAction("$BUILD_DIR/${PROGNAME}.bin", post_build_action)
