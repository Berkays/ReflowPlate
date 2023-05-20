
const profile_data_size = 24;
const profile_data_offset = 2;
const profile_name_size = 10;
const profile_padding = 28;

module.exports.parseProfileData = function (data, count) {

    const profiles = [];

    let bytePtr = 0;
    for (let i = 0; i < count; i++) {
        const profile = { data: [], name: '' };

        for (let k = 0; k < profile_data_size; k++) {
            profile.data.push(data.readUint8(bytePtr));
            bytePtr++;
        }

        for (let k = 0; k < profile_data_offset; k++)
            bytePtr++;

        const nameBuffer = Buffer.alloc(profile_name_size);
        data.copy(nameBuffer, 0, bytePtr, bytePtr + profile_name_size);

        profile.name = nameBuffer.toString('utf-8');

        profiles.push(profile);

        for (let k = 0; k < profile_padding; k++)
            bytePtr++;
    }


    return profiles;
}

module.exports.packProfileData = function (profile) {

    const profileBuffer = Buffer.alloc(profile_data_size + profile_data_offset + profile_name_size + profile_padding);

    let bytePtr = 0;
    for (let i = 0; i < profile_data_size; i++) {
        profileBuffer.writeUInt8(profile.data[i], bytePtr);
        bytePtr++;
    }

    for (let k = 0; k < profile_data_offset; k++)
        bytePtr++;

    profileBuffer.write(profile.name, bytePtr, 'utf8');

    return profileBuffer;
}