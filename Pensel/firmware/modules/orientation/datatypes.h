#pragma once

#include <stdint.h>


//! Packet header used in all packets
typedef struct __attribute__((packed)) {
    uint32_t frame_num;   //!< Frame number
    uint32_t timestamp;   //!< Timestamp when the packet was received
} pkt_header_t;


//! Raw accel packet definition
typedef struct __attribute__((packed)) {
    pkt_header_t header;  //!< Packet header
    int16_t x;            //!< raw accel X value
    int16_t y;            //!< raw accel Y value
    int16_t z;            //!< raw accel Z value
} accel_raw_t;

//! Normalized accel packet definition
typedef struct __attribute__((packed)) {
    pkt_header_t header; //!< Packet header
    float x;             //!< Normalized accel X value
    float y;             //!< Normalized accel Y value
    float z;             //!< Normalized accel Z value
} accel_norm_t;

//! Raw mag packet definition
typedef struct __attribute__((packed)) {
    pkt_header_t header;  //!< Packet header
    int16_t x;            //!< Raw mag X value
    int16_t y;            //!< Raw mag Y value
    int16_t z;            //!< Raw mag Z value
} mag_raw_t;

//! Normalized mag packet definition
typedef struct __attribute__((packed)) {
    pkt_header_t header; //!< Packet header
    float x;             //!< Normalized mag X value
    float y;             //!< Normalized mag Y value
    float z;             //!< Normalized mag Z value
} mag_norm_t;


//! Raw gyro packet definition
typedef struct __attribute__((packed)) {
    pkt_header_t header;  //!< Packet header
    int16_t x;            //!< Raw gyro X value
    int16_t y;            //!< Raw gyro Y value
    int16_t z;            //!< Raw gyro Z value
} gyro_raw_t;

//! Normalized gyro packet definition
typedef struct __attribute__((packed)) {
    pkt_header_t header; //!< Packet header
    float x;             //!< Normalized gyro X value
    float y;             //!< Normalized gyro Y value
    float z;             //!< Normalized gyro Z value
} gyro_norm_t;
