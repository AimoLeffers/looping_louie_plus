# looping_louie_plus
A modification for the table game "Looping Louie" to change its rotaion speed, without any obvious changes on the outside of the toy. 
The basic idea was to create a little suprise while playing Looping Louie.  

![front_view.jpg](https://github.com/AimoLeffers/looping_louie_plus/blob/main/front_view.jpg)

## electric wiring
The electric_diagram.png shows the complete electric rewiring. The switch and the motor of the original toy are reused. All components are hidden
inside the bottom body of the toy.

![bot_view_3_electric_wiring.jpg](https://github.com/AimoLeffers/looping_louie_plus/blob/main/bot_view_3_electric_wiring.jpg)

## software and microcontroller
The software (main.c) was written with [AtmelStudio](https://www.microchip.com/en-us/tools-resources/develop/microchip-studio) (renamed to Microchip Studio).
The electric diagram was designed with [KiCad](https://www.kicad.org/).

In order to get the software on the microcontroller i used a [development board](https://www.pollin.de/productdownloads/D810074B.PDF) from Pollin and 
the [mySmartUSB MK2b USB-Programmer](https://shop.myavr.de/index.php?sp=article.sp.php&artID=200179)
