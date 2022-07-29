const fz = require('zigbee-herdsman-converters/converters/fromZigbee');
const tz = require('zigbee-herdsman-converters/converters/toZigbee');
const exposes = require('zigbee-herdsman-converters/lib/exposes');
const reporting = require('zigbee-herdsman-converters/lib/reporting');
const extend = require('zigbee-herdsman-converters/lib/extend');
const e = exposes.presets;
const ea = exposes.access;

const definition = {
    zigbeeModel: ['LILYGO.Sensor'],
    model: 'LILYGO.Sensor',
    vendor: 'LilyGO',
    description: 'sensor_demo',
    fromZigbee: [fz.temperature, fz.humidity],
    toZigbee: [],
    exposes: [e.temperature(), e.humidity()],
    // The configure method below is needed to make the device reports on/off state changes
    // when the device is controlled manually through the button on it.
    configure: async (device, coordinatorEndpoint, logger) => {
        const endpoint = device.getEndpoint(1);
        await reporting.bind(endpoint, coordinatorEndpoint, ['msTemperatureMeasurement']);
        await reporting.temperature(endpoint);
        await reporting.bind(endpoint, coordinatorEndpoint, ['msRelativeHumidity']);
        await reporting.humidity(endpoint);
    },
};

module.exports = definition;
