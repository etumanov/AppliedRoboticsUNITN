/** @file BRO_spam_fists.h */
#ifndef __bro_headers_spam_fists_h
#define __bro_headers_spam_fists_h

/** @addtogroup BROSFists */
/* @{ */
#include "BRO_spam_client.h"

/** Decoding function for a set of BROFists.
 *
 *  This is the heart of the whole SPAM Client. It takes an array of
 *  BROFists, decodes them, then behave accordingly. After all the
 *  operations have been done the responses are used to create a number of
 *  BROFist to send back equal to that received containing all the data
 *  requested.
 *
 *  @param orders BROFist Array to be decoded.
 *  @param response BROFist Array with the results of the operations.
 *  @param motors The structure containing all the Servo Motors' Data.
 */
void decode_bro_fists (bro_fist_t * orders, bro_fist_t * response, engines_t * motors);
void updateMotors(engines_t * motors);
double rotationController(engines_t * motors);
double controller (double u_2);
double controller2 (double u_2);
double evaluate_speed(motor_t * motor, double previousSpeed);
void initialize_motors(engines_t * motor);
void manipulatePackets(bro_fist_t * orders);

/* @} */
#endif
