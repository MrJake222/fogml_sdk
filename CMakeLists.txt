cmake_minimum_required(VERSION 3.9)

project(fogml_sdk)

add_library(${PROJECT_NAME}
	anomaly_rt/fogml_lof.c
	anomaly_rt/fogml_reservoir.c
	dsp/fogml_dsp.c
	dsp/kissfft/kiss_fftr.c
	dsp/kissfft/kiss_fft.c
	ports/fogml_ports_zephyr.c
	ports/fogml_ports_riscv.c
	ports/fogml_ports_arduino.cpp
	anomaly/fogml_anomaly.c
	scaler/fogml_scaler.c
	internal/fogml_helper.c
	)

target_include_directories(${PROJECT_NAME}
	PUBLIC	.
	PRIVATE	internal
	)


