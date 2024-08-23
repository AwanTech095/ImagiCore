#include <iostream>
#include<fstream>
#include<cmath>
#include<vector>
#include<cstring>
#include<algorithm>
#include<cstdlib>

using namespace std;


struct Image {
    char ImageFileName[100];
    vector<vector<int>> ImageData;
    int cols, rows, maxGray;
    vector<char> comment;

    bool imageLoaded;
    bool imageModified;

    void changeBrightness(double factor) {
        for (int r = 0; r < rows; r++)
            for (int c = 0; c < cols; c++) {
                ImageData[r][c] *= factor;
                if (ImageData[r][c] > maxGray)
                    ImageData[r][c] = maxGray;
            }
    }

    int loadImage(char ImageName[]) {

        ifstream FCIN(ImageName);

        if (!FCIN.is_open())
            return -1;

        char MagicNumber[5];
        char Comment[100];

        FCIN.getline(MagicNumber, 4);
        FCIN.getline(Comment, 100);
        FCIN >> cols >> rows >> maxGray;

        ImageData.clear();
        ImageData.resize(rows, vector<int>(cols, 0));

        for (int r = 0; r < rows; r++)
            for (int c = 0; c < cols; c++)
                FCIN >> ImageData[r][c];

        if (FCIN.fail())
            return -2;

        FCIN.close();
        imageLoaded = true;
        imageModified = false;
        strcpy(ImageFileName, ImageName);
        return 0;
    }
    //This commented part is the original saveImage function.. we have implemented a new one so that we could add the bonus part into our code.

    /*int saveImage(char ImageName[]){
        ofstream FCOUT(ImageName);
        if(!FCOUT.is_open())
            return -1;

        FCOUT<<"P2\n# This is a comment\n"
        <<cols<<" "<<rows<<endl<<maxGray<<endl;
        for(int r = 0; r < rows; r++){
            for(int c = 0; c< cols; c++)
                FCOUT<<ImageData[r][c]<<" ";
            FCOUT<<endl;
        }
        FCOUT.close();
        imageModified = false;
        return 0;
    }
    */
    int saveImageAndView(char ImageName[]) {
        ofstream FCOUT(ImageName);
        if (!FCOUT.is_open())
            return -1;

        FCOUT << "P2\n# This is a comment\n"
            << cols << " " << rows << endl
            << maxGray << endl;
        for (int r = 0; r < rows; r++) {
            for (int c = 0; c < cols; c++)
                FCOUT << ImageData[r][c] << " ";
            FCOUT << endl;
        }
        FCOUT.close();
        imageModified = false;


        string openCommand = "start " + string(ImageName);
        system(openCommand.c_str());

        return 0;
    }

    void fliphorizontally() {
        for (int r = 0; r < rows / 2; r++) {
            for (int c = 0; c < cols; c++) {
                int T = ImageData[r][c];
                ImageData[r][c] = ImageData[rows - r - 1][c];
                ImageData[rows - r - 1][c] = T;
            }
        }
        return;
    }
    void flipvertically() {
        for (int r = 0; r < rows; ++r) {
            for (int c = 0; c < cols / 2; ++c) {
                int T = ImageData[r][c];
                ImageData[r][c] = ImageData[r][cols - c - 1];
                ImageData[r][cols - c - 1] = T;
            }
        }
        return;
    }


    void rotateImageby90() {

        vector<vector<int>> rotatedImageData(cols, vector<int>(rows, 0));


        for (int r = 0; r < rows; ++r) {
            for (int c = 0; c < cols; ++c) {
                rotatedImageData[c][rows - 1 - r] = ImageData[r][c];
            }
        }


        swap(rows, cols);


        ImageData = rotatedImageData;

        imageModified = true;
    }
    void rotateImage(double angleDegrees) {

        double angleRadians = angleDegrees * (M_PI / 180.0);
        unsigned int rotatedImageData[400][400];

        int centerX = cols / 2;
        int centerY = rows / 2;


        for (int r = 0; r < rows; ++r) {
            for (int c = 0; c < cols; ++c) {

                int translatedX = c - centerX;
                int translatedY = r - centerY;

                int rotatedX = round(translatedX * cos(angleRadians) - translatedY * sin(angleRadians));
                int rotatedY = round(translatedX * sin(angleRadians) + translatedY * cos(angleRadians));
                rotatedX += centerX;
                rotatedY += centerY;
                if (rotatedX >= 0 && rotatedX < cols && rotatedY >= 0 && rotatedY < rows) {
                    rotatedImageData[r][c] = ImageData[rotatedY][rotatedX];
                }
                else {
                    rotatedImageData[r][c] = 0;
                }
            }
        }

        for (int r = 0; r < rows; ++r) {
            for (int c = 0; c < cols; ++c) {
                ImageData[r][c] = rotatedImageData[r][c];
            }
        }

        imageModified = true;
    }
    void meanFilter(int kernelSize) {
        unsigned int filteredImageData[400][400];

        for (int r = 0; r < rows; ++r) {
            for (int c = 0; c < cols; ++c) {
                int sum = 0;
                int count = 0;

                for (int kr = -kernelSize / 2; kr <= kernelSize / 2; ++kr) {
                    for (int kc = -kernelSize / 2; kc <= kernelSize / 2; ++kc) {
                        int nr = r + kr;
                        int nc = c + kc;


                        if (nr >= 0 && nr < rows && nc >= 0 && nc < cols) {
                            sum += ImageData[nr][nc];
                            count++;
                        }
                    }
                }


                filteredImageData[r][c] = sum / count;
            }
        }


        for (int r = 0; r < rows; ++r) {
            for (int c = 0; c < cols; ++c) {
                ImageData[r][c] = filteredImageData[r][c];
            }
        }

        imageModified = true;
    }
    void linearFilter(const double kernel[3][3]) {

        unsigned int filteredImageData[400][400];


        for (int r = 0; r < rows; ++r) {
            for (int c = 0; c < cols; ++c) {
                int sum = 0;


                for (int kr = -1; kr <= 1; ++kr) {
                    for (int kc = -1; kc <= 1; ++kc) {
                        int nr = r + kr;
                        int nc = c + kc;

                        if (nr >= 0 && nr < rows && nc >= 0 && nc < cols) {

                            sum += kernel[kr + 1][kc + 1] * ImageData[nr][nc];
                        }
                    }
                }

                filteredImageData[r][c] = sum;
            }
        }


        for (int r = 0; r < rows; ++r) {
            for (int c = 0; c < cols; ++c) {
                ImageData[r][c] = filteredImageData[r][c];
            }
        }

        imageModified = true;
    }
    void shrink(double shrinkageFactor) {

        for (int r = 0; r < rows; ++r) {
            for (int c = 0; c < cols; ++c) {

                ImageData[r][c] = static_cast<int>(ImageData[r][c] * shrinkageFactor);


                ImageData[r][c] = static_cast<unsigned int>(max(0, min(static_cast<int>(maxGray), static_cast<int>(ImageData[r][c]))));
            }
        }

        imageModified = true;
        cout << "Shrinkage filter applied. You need to save the changes." << endl;
    }


    void resizeImageNonUniform(int newWidth, int newHeight) {
        vector<vector<int>> resizedImageData(newHeight, vector<int>(newWidth, 0));

        double rowScale = static_cast<double>(rows) / newHeight;
        double colScale = static_cast<double>(cols) / newWidth;

        for (int r = 0; r < newHeight; ++r) {
            for (int c = 0; c < newWidth; ++c) {
                int originalRow = static_cast<int>(r * rowScale);
                int originalCol = static_cast<int>(c * colScale);

                resizedImageData[r][c] = ImageData[originalRow][originalCol];
            }
        }

        rows = newHeight;
        cols = newWidth;

        ImageData = resizedImageData;

        imageModified = true;
    }

    void translateImage(int deltaX, int deltaY) {
        unsigned int translatedImageData[400][400];

        for (int r = 0; r < rows; ++r) {
            for (int c = 0; c < cols; ++c) {

                int originalRow = r - deltaY;
                int originalCol = c - deltaX;


                if (originalRow >= 0 && originalRow < rows && originalCol >= 0 && originalCol < cols) {

                    translatedImageData[r][c] = ImageData[originalRow][originalCol];
                }
                else {

                    translatedImageData[r][c] = 0;
                }
            }
        }


        for (int r = 0; r < rows; ++r) {
            for (int c = 0; c < cols; ++c) {
                ImageData[r][c] = translatedImageData[r][c];
            }
        }

        imageModified = true;
    }
    void linearContrastStretching() {
        double minPixelValue = static_cast<double>(ImageData[0][0]);
        double maxPixelValue = static_cast<double>(ImageData[0][0]);


        for (int r = 0; r < rows; ++r) {
            for (int c = 0; c < cols; ++c) {
                if (ImageData[r][c] < minPixelValue) {
                    minPixelValue = static_cast<double>(ImageData[r][c]);
                }
                if (ImageData[r][c] > maxPixelValue) {
                    maxPixelValue = static_cast<double>(ImageData[r][c]);
                }
            }
        }


        for (int r = 0; r < rows; ++r) {
            for (int c = 0; c < cols; ++c) {

                double scaledValue = (ImageData[r][c] - minPixelValue) * (maxGray / (maxPixelValue - minPixelValue));


                ImageData[r][c] = static_cast<unsigned int>(max(0.0, min(scaledValue, static_cast<double>(maxGray))));
            }
        }

        imageModified = true;
        cout << "Linear contrast stretching applied. You need to save the changes." << endl;
    }
    void scalingImage(double scaleFactor) {
        // Calculate the new dimensions after scaling
        int newRows = static_cast<int>(round(rows * scaleFactor));
        int newCols = static_cast<int>(round(cols * scaleFactor));


        vector<vector<int>> scaledImageData(newRows, vector<int>(newCols, 0));


        for (int r = 0; r < newRows; ++r) {
            for (int c = 0; c < newCols; ++c) {

                int originalRow = static_cast<int>(round(r / scaleFactor));
                int originalCol = static_cast<int>(round(c / scaleFactor));

                originalRow = max(0, min(originalRow, rows - 1));
                originalCol = max(0, min(originalCol, cols - 1));


                scaledImageData[r][c] = ImageData[originalRow][originalCol];
            }
        }


        rows = newRows;
        cols = newCols;


        ImageData = scaledImageData;

        imageModified = true;
    }
    void converttobinaryImage(int threshold)
    {
        for (int r = 0; r < rows; r++)
        {
            for (int c = 0; c < cols; c++)
            {
                if (ImageData[r][c] < threshold)
                    ImageData[r][c] = 0;
                else
                    ImageData[r][c] = 255;
            }
        }
    }
    void sharpenimage(double strength) {

        double laplacianKernel[3][3] = { {-1, -1, -1},
                                         {-1,  9, -1},
                                         {-1, -1, -1} };


        unsigned int sharpenedImageData[400][400];


        for (int r = 0; r < rows; ++r) {
            for (int c = 0; c < cols; ++c) {
                int sum = 0;


                for (int kr = -1; kr <= 1; ++kr) {
                    for (int kc = -1; kc <= 1; ++kc) {
                        int nr = r + kr;
                        int nc = c + kc;


                        if (nr >= 0 && nr < rows && nc >= 0 && nc < cols) {

                            sum += laplacianKernel[kr + 1][kc + 1] * ImageData[nr][nc];
                        }
                    }
                }

                sharpenedImageData[r][c] = ImageData[r][c] + static_cast<int>(strength * sum);
            }
        }


        for (int r = 0; r < rows; ++r) {
            for (int c = 0; c < cols; ++c) {
                ImageData[r][c] = static_cast<unsigned int>(std::max(0, std::min(static_cast<int>(maxGray), static_cast<int>(sharpenedImageData[r][c]))));
            }
        }

        imageModified = true;
        cout << "Image sharpened. You need to save the changes." << endl;
    }
    void cropImage(int startX, int startY, int width, int height) {
        vector<vector<int>> croppedImage(height, vector<int>(width));
        for (int r = 0; r < height; ++r) {
            for (int c = 0; c < width; ++c) {
                if (startY + r < rows && startX + c < cols) {
                    croppedImage[r][c] = ImageData[startY + r][startX + c];
                }
            }
        }
        ImageData = croppedImage;
        rows = height;
        cols = width;
    }
    void combineImagesSideBySide(Image& otherImage) {
        int newCols = cols + otherImage.cols;
        int newRows = max(rows, otherImage.rows);
        vector<vector<int>> combinedImage(newRows, vector<int>(newCols, 0));

        for (int r = 0; r < rows; ++r) {
            for (int c = 0; c < cols; ++c) {
                combinedImage[r][c] = ImageData[r][c];
            }
        }


        for (int r = 0; r < otherImage.rows; ++r) {
            for (int c = 0; c < otherImage.cols; ++c) {
                combinedImage[r][c + cols] = otherImage.ImageData[r][c];
            }
        }

        ImageData = combinedImage;
        rows = newRows;
        cols = newCols;
    }
    void combineImagesTopToBottom(const Image& otherImage) {
        int newRows = rows + otherImage.rows;
        int newCols = max(cols, otherImage.cols);
        vector<vector<int>> combinedImage(newRows, vector<int>(newCols, 0));


        for (int r = 0; r < rows; ++r) {
            for (int c = 0; c < cols; ++c) {
                combinedImage[r][c] = ImageData[r][c];
            }
        }

        for (int r = 0; r < otherImage.rows; ++r) {
            for (int c = 0; c < otherImage.cols; ++c) {
                combinedImage[rows + r][c] = otherImage.ImageData[r][c];
            }
        }


        ImageData = combinedImage;
        rows = newRows;
        cols = newCols;
        imageModified = true;
    }
    void medianFilter(int kernelSize) {
        unsigned int filteredImageData[400][400];

        for (int r = 0; r < rows; ++r) {
            for (int c = 0; c < cols; ++c) {
                unsigned int values[400];
                int count = 0;


                for (int kr = -kernelSize / 2; kr <= kernelSize / 2; ++kr) {
                    for (int kc = -kernelSize / 2; kc <= kernelSize / 2; ++kc) {
                        int nr = r + kr;
                        int nc = c + kc;


                        if (nr >= 0 && nr < rows && nc >= 0 && nc < cols) {
                            values[count++] = ImageData[nr][nc];
                        }
                    }
                }


                sort(values, values + count);


                filteredImageData[r][c] = values[count / 2];
            }
        }

        for (int r = 0; r < rows; ++r) {
            for (int c = 0; c < cols; ++c) {
                ImageData[r][c] = filteredImageData[r][c];
            }
        }

        imageModified = true;
    }
    void computeDerivative() {

        int derivative[400][400];
        for (int r = 1; r < rows - 1; ++r) {
            for (int c = 1; c < cols - 1; ++c) {

                int dx = ImageData[r][c + 1] - ImageData[r][c - 1];
                int dy = ImageData[r + 1][c] - ImageData[r - 1][c];


                derivative[r][c] = static_cast<unsigned int>(sqrt(dx * dx + dy * dy));
            }
        }

        for (int r = 1; r < rows - 1; ++r) {
            for (int c = 1; c < cols - 1; ++c) {
                ImageData[r][c] = derivative[r][c];
            }
        }

        imageModified = true;
    }
    void findEdges() {

        int sobelX[3][3] = { {-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1} };
        int sobelY[3][3] = { {-1, -2, -1}, {0, 0, 0}, {1, 2, 1} };


        int gradientX[400][400];
        int gradientY[400][400];


        for (int r = 1; r < rows - 1; ++r) {
            for (int c = 1; c < cols - 1; ++c) {
                int gx = 0, gy = 0;

                for (int kr = -1; kr <= 1; ++kr) {
                    for (int kc = -1; kc <= 1; ++kc) {
                        gx += sobelX[kr + 1][kc + 1] * ImageData[r + kr][c + kc];
                    }
                }


                for (int kr = -1; kr <= 1; ++kr) {
                    for (int kc = -1; kc <= 1; ++kc) {
                        gy += sobelY[kr + 1][kc + 1] * ImageData[r + kr][c + kc];
                    }
                }


                gradientX[r][c] = gx;
                gradientY[r][c] = gy;
            }
        }


        for (int r = 1; r < rows - 1; ++r) {
            for (int c = 1; c < cols - 1; ++c) {

                ImageData[r][c] = static_cast<unsigned int>(sqrt(gradientX[r][c] * gradientX[r][c] + gradientY[r][c] * gradientY[r][c]));
            }
        }

        imageModified = true;
    }
    void imageprocess() {
        for (int r = 0; r < rows; r++) {
            for (int c = 0; c < cols / 4; c++) {
                int T = ImageData[r][c];
                ImageData[r][c] = ImageData[r][cols / 2 - c];
                ImageData[r][cols / 2 - c] = T;
                T = ImageData[r][cols / 2 + c + 1];
                ImageData[r][cols / 2 + c + 1] = ImageData[r][cols - c];
                ImageData[r][cols - c] = T;


            }
        }
    }


};

struct Menu {
    vector<string> menuItems;

    Menu(char menuFile[]) {
        loadMenu(menuFile);
    }

    int loadMenu(char menuFile[]) {
        ifstream IN;
        IN.open(menuFile);
        if (!IN.is_open())
            return -1;
        char menuItem[100], TotalItems[10];

        int Choices;

        IN.getline(TotalItems, 8);
        Choices = atoi(TotalItems);
        for (int i = 1; i <= Choices; i++) {
            IN.getline(menuItem, 99);
            menuItems.push_back(menuItem);
        }
        IN.close();
        return Choices;
    }

    int presentMenu() {
        int userChoice;
        int totalChoices = menuItems.size();


        do {
            int k = 1;
            for (int i = 0; i < totalChoices; i++) {
                if (menuItems[i][0] != '*') {
                    cout << k << "\t" << menuItems[i] << endl;
                    k++;
                }
            }
            cout << " Enter Your Choice (1 - " << k - 1 << " ) ";
            cin >> userChoice;
        } while (userChoice < 1 || userChoice > totalChoices);
        return userChoice;
    }



};

int main() {
    char MenuFile[] = "C:\\TEMP\\MainMenu.txt";
    Image Images[2];
    int ActiveImage = 0;
    int errorCode = 0;
    int userChoice;
    //    int TotalChoices = loadMenu("MainMenu.txt");
    int totalChoices;

    Menu menu(MenuFile);
    totalChoices = menu.menuItems.size();
    do {
        userChoice = menu.presentMenu();
        if (1 == userChoice) {
            char ImageFileName[100];
            cout << "Specify File Name ";
            cin >> ImageFileName;
            errorCode = Images[ActiveImage].loadImage(ImageFileName);
            if (errorCode == 0) {
                cout << "File Loaded Successfully " << endl;
            }
            else {
                cout << "Load Error: Code " << errorCode << endl;
            }
        }
        else if (2 == userChoice) {
            char ImageFileName[100];
            cout << "Specify File Name ";
            cin >> ImageFileName;
            errorCode = Images[ActiveImage].saveImageAndView(ImageFileName);
            // ...
        }

        else if (3 == userChoice) {
            Images[ActiveImage].fliphorizontally();
            cout << "You need to save the changes " << endl;
        }
        else if (4 == userChoice) {
            Images[ActiveImage].flipvertically();
            cout << "You need to save the changes " << endl;
        }
        else if (5 == userChoice) {
            Images[ActiveImage].rotateImageby90();
            cout << "Image rotated by 90 degrees. You need to save the changes." << endl;

        }
        else if (userChoice == 6) {
            double angle;
            cout << "Enter the angle to rotate the image (in degrees): ";
            cin >> angle;

            Images[ActiveImage].rotateImage(angle);
            cout << "Image rotated. You need to save the changes." << endl;

        }
        else if (userChoice == 7) {
            // Apply mean filter
            int kernelSize;
            cout << "Enter the size of the mean filter kernel : ";
            cin >> kernelSize;

            Images[ActiveImage].meanFilter(kernelSize);
            cout << "Mean filter applied. You need to save the changes." << endl;
        }
        else if (8 == userChoice)
        {
            // Apply median filter
            int kernelSize;
            cout << "Enter the size of the median filter kernel : ";
            cin >> kernelSize;

            Images[ActiveImage].medianFilter(kernelSize);
            cout << "Median filter applied. You need to save the changes." << endl;

        }
        else if (userChoice == 9) {
            // Apply linear filter
            double kernel[3][3];

            cout << "Enter the 3x3 linear filter kernel values:" << endl;
            cout << "For better results, Input values in the kernel ranging from 0.1 to 0.5. " << endl;
            for (int i = 0; i < 3; ++i) {
                for (int j = 0; j < 3; ++j) {
                    cout << "Kernel[" << i << "][" << j << "]: ";
                    cin >> kernel[i][j];
                }
            }
            Images[ActiveImage].linearFilter(kernel);
            cout << "Linear filter applied. You need to save the changes." << endl;
        }
        else if (userChoice == 10) {

            int newRows, newCols;
            cout << "Enter the new number of rows: ";
            cin >> newRows;
            cout << "Enter the new number of columns: ";
            cin >> newCols;

            Images[ActiveImage].resizeImageNonUniform(newRows, newCols);
            cout << "Image resized. You need to save the changes." << endl;
        }
        else if (userChoice == 11) {
            int deltaX, deltaY;
            cout << "Enter the translation in X direction: ";
            cin >> deltaX;
            cout << "Enter the translation in Y direction: ";
            cin >> deltaY;

            Images[ActiveImage].translateImage(deltaX, deltaY);
            cout << "Image translated. You need to save the changes." << endl;
        }
        else if (userChoice == 18) {
            Images[ActiveImage].linearContrastStretching();
        }
        else if (12 == userChoice)
        {
            double scaleFactor;
            cout << "Enter the scaling factor: ";
            cin >> scaleFactor;

            Images[ActiveImage].scalingImage(scaleFactor);
            cout << "Image scaled. You need to save the changes." << endl;
        }
        else if (userChoice == 13) {
            int threshold;
            cout << "Enter a threshold value: ";
            cin >> threshold;
            Images[ActiveImage].converttobinaryImage(threshold);
            cout << "Image has been converted to Binary. You need to save the changes." << endl;

        }
        else if (userChoice == 15) {
            double strength;
            cout << "Enter the strength of sharpening: ";
            cin >> strength;

            Images[ActiveImage].sharpenimage(strength);

        }
        else if (userChoice == 20) {
            int startX, startY, width, height;
            cout << "Enter start X coordinate: ";
            cin >> startX;
            cout << "Enter start Y coordinate: ";
            cin >> startY;
            cout << "Enter width: ";
            cin >> width;
            cout << "Enter height: ";
            cin >> height;
            Images[ActiveImage].cropImage(startX, startY, width, height);
            cout << "Image cropped. You need to save the changes." << endl;

        }
        else if (userChoice == 21) {
            Image anotherImage;

            // Example: Load an image into the new object
            char AnotherImageFileName[100];
            cout << "Specify Another File Name: ";
            cin >> AnotherImageFileName;

            int errorCode = anotherImage.loadImage(AnotherImageFileName);

            if (errorCode == 0) {
                cout << "Another File Loaded Successfully." << endl;
            }
            else {
                cout << "Load Error: Code " << errorCode << endl;
            }

            // Example: Combine the two images side by side
            Images[ActiveImage].combineImagesSideBySide(anotherImage);
        }
        else if (userChoice == 22) {
            Image anotherImage;

            // Example: Load an image into the new object
            char AnotherImageFileName[100];
            cout << "Specify Another File Name: ";
            cin >> AnotherImageFileName;

            int errorCode = anotherImage.loadImage(AnotherImageFileName);

            if (errorCode == 0) {
                cout << "Another File Loaded Successfully." << endl;
            }
            else {
                cout << "Load Error: Code " << errorCode << endl;

            }
            Images[ActiveImage].combineImagesTopToBottom(anotherImage);

        }
        else if (userChoice == 16) {
            double factor;
            cout << "Enter shrink factor: ";
            cin >> factor;

            Images[ActiveImage].shrink(factor);

        }

        else if (14 == userChoice) {
            int factor;
            cout << "Enter factor of brightness: " << endl;
            cout << "For better results, enter a factor ranging from 1 to 3 :";
            cin >> factor;
            Images[ActiveImage].changeBrightness(factor);
            cout << "You need to save the changes " << endl;
        }
        else if (17 == userChoice)
        {

            Images[ActiveImage].findEdges();
            cout << "Edges detected using derivatives. You need to save the changes." << endl;
        }
        else if (userChoice == 19) {
            Images[ActiveImage].computeDerivative();
            cout << "Derivative filter applied. You need to save the changes" << endl;
        }
        else if (userChoice == 23) {
            Images[ActiveImage].imageprocess();
            cout << "Image reverted. You need to save the changes." << endl;

        }

    } while (userChoice != totalChoices);
    return 0;
}
