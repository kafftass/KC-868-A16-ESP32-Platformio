Hi folks , i had problems whit the LAN8720 Chip which is integrated into the KC-868 A16 Board. I had always fast connection losses and other issues , also didnt work with WlAN & Lan together ON.
After i had read a lot of incompatibilities between Arduino 2 and 3 Core i tried different older Arduino cores and found that ESP-IDF 4.4.0 is the best solution for me.
This problem only exist on platformio as much as i know, because its not supporting Ardunio 3 cores. My tries to use other 3er core sources were not successfull because of to much errors occure in my existing code.
Replace your platform entry in platform IO with that :
platform = espressif32@4.4.0 
Good luck / Greetz
