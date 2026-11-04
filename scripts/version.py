import json
data = json.load(open('version.json'))
print(f"-D BAMBUDECK_FIRMWARE_VERSION=\\\"{data['version']}\\\"")
