Various tests component tests used during development of the Arduino firmware for FPT Driverless Car.
They could be used to test components, although the main firmware should have supported most test cases already.

battery_test: Drive in circle
encoder_and_esc_test: start the engine and log encoder periodically
encoder_count_to_distance: test to measure the relation between encoder count and distance travelled
encoder_test: Output the Encoder count when it is changed
pid: Some tests to develop and tune PID
sonar_test: Poll the sonar and display the result repeatedly
steering_test: Control PCA9685 to drive the steering servo. Need to connect PCA9685 to UNO via I2C
test_button: blink LED slowly when button released, quickly when button pushed
test_esc_servo_output_fixed: start Arduino, within 5s start ESC, engine should start running
test_esc_servo_output_from_serial: receive ESC control value from serial

