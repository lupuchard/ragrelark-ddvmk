#ifndef BLOBBER_H
#define BLOBBER_H


class Blobber {
    public:
        Blobber();
        virtual ~Blobber();

        void makeCircle(int rad);
        void makeCircleOfSize(int size);

        const bool** getBlob();
        unsigned short getWidth();
        unsigned short getHeight();
    protected:
    private:
        bool** blobArr;
        unsigned short blobWidth;
        unsigned short blobHeight;
};
#endif // BLOBBER_H
