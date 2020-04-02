#include <usb_names.h>

#define MANUFACTURER_NAME    {'1','0','x','1','0'}
#define MANUFACTURER_NAME_LEN 5
#define PRODUCT_NAME { 'M','I','D','I','B','o','a','r','d',' ','L','C'}
#define PRODUCT_NAME_LEN 12

struct usb_string_descriptor_struct usb_string_manufacturer_name = {
        2 + MANUFACTURER_NAME_LEN * 2,
        3,
        MANUFACTURER_NAME
};

struct usb_string_descriptor_struct usb_string_product_name = {
        2 + PRODUCT_NAME_LEN * 2,
        3,
        PRODUCT_NAME
};
