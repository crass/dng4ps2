## NOTE: This is here for historical and informational reasons. I don't have time to process the images into the color profile.  If you do, you can add the profile to [[CameraOpts.cpp|https://github.com/crass/dng4ps2/blob/master/src/CameraOpts.cpp#L82]] and send a pull request.  More comprehensive instructions for creating the color profile can be found in these [instructions](http://chdk.wikia.com/wiki/DNG4PS-2/Calibration).

### Don't you like colors on your images or your camera model is not supported by dng4ps2 yet?
If you don't like colors on your images after developing DNG files, possibly the color profile for your camera model has not been calculated. You can help author of dng4ps2 to calculate color profile for your camera. You have to do:
* take a picture of special image from your PC display (see below)
* compress RAW and Jpeg file
* upload archive to a publicly accessible hosting service
* send archive link to my email address

__Important!__ Before taking picture you need to do the following:

#### On a PC
1.    Download this [image](https://github.com/crass/dng4ps2/raw/master/docs/pallete.PNG) and save it on your PC
1.    Open it with full screen mode

#### with camera

1.    Select "Program" mode
1.    Set best jpeg quality
1.    Set white balance to "day light"
1.    Switch off all effects
1.    Set exposure compensation to -2/3
1.    Set focus to infinity
1.    Set zoom to middle
1.    Set flash to off
1.    Set ISO to lowest value
1.    And don't take a picture in dark room. Such photos give poor results because of big contrast!

