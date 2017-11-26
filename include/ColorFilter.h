// Johannes Feyrer (jo.feyrer@gmail.com) / 2016 / FreiburgRaceCars

#ifndef COLORFILTER_H_
#define COLORFILTER_H_

// TODO: implement if needed.
// This class is meant to filter an image by it's color.
// It is only a rudimentary skeleton by now since extracting RGB channels
// is enough for few cars.
class ColorFilter {

public:

    // constructor
    ColorFilter();

    // extracts images of red, green, blue and yellow objects from the input image
    int filter(Mat input);

    // outputs colored object images to extern
    Mat getRed();
    Mat getGreen();
    Mat getBlue();
    Mat getYellow();

private:

    // original image
    Mat image;

    // rgb channels from raw image
    Mat r, g, b;

    // bitwise_and of (r and b), (r, b and g) in order to subtract background
    Mat rnb, rnbng, backsub;

    // rgb channels with subtracted background
    Mat r_, g_, b_;

    // bitwise_not of rgb channels with subtracted background in order to calculate channels with pure r, g, b and yellow objects
    Mat b_n, g_n, r_n;

    // channels with pure r, g, b and yellow objects
    Mat blue, red, yellow, green;

    // needed as output by openCV to split channels
    Mat channels[3];

};

#endif // COLORFILTER_H_
