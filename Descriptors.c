#include "Descriptors.h"

const USB_Descriptor_Device_t PROGMEM DeviceDescriptor =
	{
		.Header = {.Size = sizeof(USB_Descriptor_Device_t), .Type = DTYPE_Device},

		.USBSpecification = VERSION_BCD(1, 0, 1),
		.Class = USB_CSCP_NoDeviceClass,
		.SubClass = USB_CSCP_NoDeviceSubclass,
		.Protocol = USB_CSCP_NoDeviceProtocol,

		.Endpoint0Size = FIXED_CONTROL_ENDPOINT_SIZE,

		.VendorID = 0x0000,
		.ProductID = 0x7305,
		.ReleaseNumber = 0x0000,

		.ManufacturerStrIndex = 0,
		.ProductStrIndex = 0,
		.SerialNumStrIndex = 0,

		.NumberOfConfigurations = FIXED_NUM_CONFIGURATIONS};

const USB_Descriptor_Configuration_t PROGMEM ConfigurationDescriptor =
	{
		.Config =
			{
				.Header = {.Size = sizeof(USB_Descriptor_Configuration_Header_t), .Type = DTYPE_Configuration},

				.TotalConfigurationSize = sizeof(USB_Descriptor_Configuration_t),
				.TotalInterfaces = 1,

				.ConfigurationNumber = 1,
				.ConfigurationStrIndex = 0,

				.ConfigAttributes = (USB_CONFIG_ATTR_RESERVED | USB_CONFIG_ATTR_SELFPOWERED),

				.MaxPowerConsumption = USB_CONFIG_POWER_MA(100)},

		.CDC_DCI_Interface =
			{
				.Header = {.Size = sizeof(USB_Descriptor_Interface_t), .Type = DTYPE_Interface},

				.InterfaceNumber = 0,
				.AlternateSetting = 0,

				.TotalEndpoints = 3,

				.Class = CDC_CSCP_NoSpecificSubclass,
				.SubClass = CDC_CSCP_NoSpecificSubclass,
				.Protocol = CDC_CSCP_NoSpecificProtocol,

				.InterfaceStrIndex = NO_DESCRIPTOR},

		.CDC_DataInterruptEndpoint =
			{
				.Header = {.Size = sizeof(USB_Descriptor_Endpoint_t), .Type = DTYPE_Endpoint},

				.EndpointAddress = CDC_NOTIFICATION_EPADDR,
				.Attributes = (EP_TYPE_INTERRUPT | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA),
				.EndpointSize = CDC_NOTIFICATION_EPSIZE,
				.PollingIntervalMS = 3},

		.CDC_DataInEndpoint =
			{
				.Header = {.Size = sizeof(USB_Descriptor_Endpoint_t), .Type = DTYPE_Endpoint},

				.EndpointAddress = CDC_TX_EPADDR,
				.Attributes = (EP_TYPE_BULK | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA),
				.EndpointSize = CDC_TXRX_EPSIZE,
				.PollingIntervalMS = 10},

		.CDC_DataOutEndpoint =
			{
				.Header = {.Size = sizeof(USB_Descriptor_Endpoint_t), .Type = DTYPE_Endpoint},

				.EndpointAddress = CDC_RX_EPADDR,
				.Attributes = (EP_TYPE_BULK | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA),
				.EndpointSize = CDC_TXRX_EPSIZE,
				.PollingIntervalMS = 10},
};

const USB_Descriptor_String_t PROGMEM BlankString =
	{
		.Header = {.Size = USB_STRING_LEN(1), .Type = DTYPE_String},

		.UnicodeString = L""};

uint16_t CALLBACK_USB_GetDescriptor(const uint16_t wValue,
									const uint16_t wIndex,
									const void **const DescriptorAddress)
{
	const uint8_t DescriptorType = (wValue >> 8);
	const uint8_t DescriptorNumber = (wValue & 0xFF);

	const void *Address = NULL;
	uint16_t Size = NO_DESCRIPTOR;

	switch (DescriptorType)
	{
	case DTYPE_Device:
		Address = &DeviceDescriptor;
		Size = sizeof(USB_Descriptor_Device_t);
		break;
	case DTYPE_Configuration:
		Address = &ConfigurationDescriptor;
		Size = sizeof(USB_Descriptor_Configuration_t);
		break;
	case DTYPE_String:
		Address = &BlankString;
		Size = pgm_read_byte(&BlankString.Header.Size);
		break;
	}

	*DescriptorAddress = Address;
	return Size;
}
