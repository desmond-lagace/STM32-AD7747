# STM32-AD7747

For use with STM32CubeIDE and the HAL library. Ensure that the `main.h` file for your STM32 MCU is included in your working directory.

## Example

Initialization:

```cpp
AD7747 ad7747 = AD7747();
if ( !ad7747.begin(&hi2c1) ) {
  while(1);
}
```

Reading capacitance and temperature data:

```cpp
float C = ad7747.getCapacitance(&hi2c);
float T = ad7747.getTempC(&hi2c);
```

## CAPDAC

CAPDAC A subtracts an offset value from the input capacitance on CIN(+), and CAPDAC B does the same for CIN(-).
The approximate resulting data value is:

$DATA \approx (C_{+} - CAPDAC_A) - (C_{-} - CAPDAC_B)$

Set the CAPDAC value by using the `setCAPDAC` function. Maximum input value is 8.192. Example usage:

```cpp
/*  Set CAPDAC A to 8.192 pF  */
ad7747.setCAPDAC(&hi2c1,A,8.192);
```

## Notes

Default configuration is 22 ms conversion time, 45.5 Hz update rate, continuous conversion mode.
Set the conversion mode with the `setConversionMode` function, with choices being `IDLE`, `CONTINUOUS`, or `SINGLE`. Example usage:

```cpp
/*  Set conversion mode to SINGLE  */
ad7747.setConversionMode(&hi2c1,SINGLE);
```
