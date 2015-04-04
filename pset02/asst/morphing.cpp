// morphing.cpp
// Assignment 2


#include "morphing.h"
using namespace std;

/**************************************************************
 *            IMAGE WARPING/MORPHING FUNCTIONS                *
 *************************************************************/

// PS02 - 4.3.1: warp an entire image according to a pair of segments.
Image warpBy1(const Image &im, Segment &segBefore, Segment &segAfter){
    float segAfterU, segAfterV;
    int width, height, channels;
    width = im.width();
    height = im.height();
    channels = im.channels();
    Image output(width, height, channels);
    int i, j, k;
    for (i = 0; i < width; i++) {
       for (j = 0; j < height; j++) {
         float fi = (float) i;
         float fj = (float) j;
         segAfterU = segAfter.getU(fi,fj);
         segAfterV = segAfter.getV(fi,fj);
         vector<float> X = segBefore.UVtoX(segAfterU, segAfterV);
         float x = X[0];
         float y = X[1];
         for (k = 0; k < channels; k++) {
           output(i, j,k) = interpolateLin(im, x, y, k, true); 
         }
       }
    }
    return output;
}

// PS02 - 4.4.2: warp an image according to a vector of before and after segments using segment weighting
Image warp(const Image &im, vector<Segment> &segsBefore, vector<Segment> &segsAfter, float a, float b, float p){
    
    int i, j, k;
    vector<float> dsum(2,0);
    int width, height, channels, numSegs;
    float weightsum, u, v;
    width = im.width();
    height = im.height();
    channels = im.channels();
    numSegs = segsBefore.size();
    Image output(width, height, channels);
    // For each pixel X in destination
    for (i = 0; i < width; i++) {
      for (j = 0; j < height; j++) {
        // DSUM = 0
        // weightsum = 0
        dsum[0] = 0; dsum[1] = 0; weightsum = 0;
        float fi = (float) i; float fj = (float) j;
        // For each line PiQi
        for (k = 0; k < numSegs; k++) {
          // calculate u, v based on PiQi
          vector<float> X(2,0);
          X[0] = fi; X[1] = fj;
          Segment after = segsAfter[k];
          Segment before = segsBefore[k];
          u = after.getU(fi, fj);
          v = after.getV(fi, fj);
          // Calculate X'i based on Pi'Qi'
          vector<float> XP = before.UVtoX(u,v);
          // Di = X'i - Xi
          vector<float> displacement(2,0);
          displacement[0] = XP[0] - X[0];
          displacement[1] = XP[1] - X[1];
          // Dist and weight calculated
          float weight = after.weight(fi,fj,a,b,p);
          // DSUM += Di * weight
          dsum[0] += displacement[0] * weight;
          dsum[1] += displacement[1] * weight;
          // weightsum += weight
          weightsum += weight;
        }
        dsum[0] /= weightsum; dsum[1] /= weightsum;
        float xf = fi + dsum[0];
        float yf = fj + dsum[1];
        int c;
        for (c = 0; c < channels; c++) {
          output(i,j,c) = interpolateLin(im, xf, yf, c, true);
        } 
      }
    }

return output;
}

//PS02 - 4.5.1: return a vector of N images in addition to the two inputs that morphs going between im1 and im2 for the corresponding segments
vector<Image> morph(const Image &im1, const Image &im2, vector<Segment> &segsBefore, vector<Segment> &segsAfter, int N, float a, float b, float p){
    if (im1.number_of_elements() == im2.number_of_elements()) {
      
    } else {
      throw MismatchedSizeException();
    }
    N += 2;
    float timeStep = 1.0 / (N + 1);
    vector<Image> sequence(N,0);
    sequence[0] = im1;
    sequence[(N-1)] = im2;
    int i, j; // loop indices
    float newX1, newX2, newY1, newY2;
    //cout << "The value of N: " << N << endl;
    for (i = 1; i < N - 1; i++) {
      float time = timeStep * (i);

      // Loop through all segment. Create new list of segsIntermediate
      int numSegs = segsBefore.size();
      vector<Segment> segsInter;
      for (j = 0; j < numSegs; j++) {
        newX1 = (1 - time) * segsBefore[j].P[0] + time*segsAfter[j].P[0];
        newY1 = (1 - time) * segsBefore[j].P[1] + time*segsAfter[j].P[1];
        newX2 = (1 - time) * segsBefore[j].Q[0] + time*segsAfter[j].Q[0];
        newY2 = (1 - time) * segsBefore[j].Q[1] + time*segsAfter[j].Q[1];
        Segment newSeq(newX1, newY1, newX2, newY2);
        segsInter.push_back(newSeq);
      }
      //cout << "Made it to where the warp begins." << endl;
      // Warp the images
      Image output1 = warp(im1, segsBefore, segsInter, a, b, p);
      //output1.debug_write();
      //cout << "Made it past one warp." << endl;
      Image output2 = warp(im2, segsAfter, segsInter, a, b, p);
      //output2.debug_write();
      Image finalOutput(output1.width(), output1.height(), output1.channels());
      // Overlay images
      int r, c, z;
      for (r = 0; r < output1.width(); r++) {
        for(c = 0; c < output1.height(); c++) {
          for(z = 0; z < output1.channels(); z++) {
            finalOutput(r,c,z) = (1 - time) * output1(r,c,z) + time* output2(r,c,z);
          }
        }
      }
      //cout << "Made it to sequence.";
      sequence[i] = finalOutput;
      //cout << " And we made it past sequence." << endl;
    }
    //cout << "And we are out of the loop." << endl;
    return sequence;
}

/**************************************************************
 *                 SEGMENT CLASS FUNCTIONS                    *
 *************************************************************/

// PS02 - 4.2.1: Segment constructor takes in 2 points (x1,y1) and (x2,y2) correspoding to the ends of a segment and computes:
// P - 2-element vector to point (x1, y1)
// Q - 2-element vector to pont (x2, y2)
// PQ - 2-element vector from P to Q
// PQ2 - float containing squared distance between P and Q
// PQlength - float containing distance between P and Q
// PQDivByPQlength2 - 2-element vector PQ normalized by PQ2
// perpPQ - 2-element vector perpendicular to PQ
// perpPQDivByPQlength - 2-element vector perpPQ normalized by PQlength
Segment::Segment(const float &x1, const float &y1, const float &x2, const float &y2) {

    P       = vector<float>(2,0);
    Q       = vector<float>(2,0);
    perpPQ  = vector<float>(2,0);

    // finish implementing this
    P[0] = x1;
    P[1] = y1;
    Q[0] = x2;
    Q[1] = y2;
    PQ = subtract(P, Q);
    PQ2 = dot(PQ, PQ);
    PQlength = sqrt(PQ2);
    float scalar = 1.0 / PQ2;
    PQDivByPQlength2 = scalarMult(PQ, scalar);
    perpPQ[0] = -1 * PQ[1];
    perpPQ[1] = PQ[0];
    scalar = 1.0 / PQlength;
    perpPQDivByPQlength = scalarMult(perpPQ, scalar);
}


// PS02 - 4.2.2: Implement the computation of the u coordinate of a point (x,y) with respect to a segment
float Segment::getU(float &x, float &y){
    Segment PX(P[0], P[1], x, y);
    float dotProd = dot(PX.PQ, PQ);
    return dotProd / PQ2;
}


// PS02 - 4.2.2: Implement the computation of the v coordinate of a point (x,y) with respect to a segment
float Segment::getV(float &x, float &y){
    Segment PX(P[0], P[1], x, y);
    vector<float> PXVector = PX.PQ;
    return dot(perpPQDivByPQlength, PXVector);
}


// PS02 - 4.2.2: compute the new (x, y) position of a point given by the (u,v) location relative to another segment.
// return the point (x,y) in a 2-element vector
vector<float> Segment::UVtoX(float &u, float &v){
    // X = P + u*PQ + v * perpPQ / |PQ|
    // to lazy for add
    // X = P - (-uPQ) - (-vPerp / PQ)
    float neg = -1.0;
    vector<float> uPQ = scalarMult(PQ, u);
    vector<float> vPerp = scalarMult(perpPQDivByPQlength, v);
    vector<float> negUPQ = scalarMult(uPQ, neg);
    vector<float> negVPerp = scalarMult(vPerp, neg);
    vector<float> output = subtract(negUPQ, P);
    output = subtract(negVPerp, output);
    return output;
}

// PS02 - 4.2.3: Implement distance from a point (x,y) to the segment. Remember the 3 cases from class
float Segment::dist(float &x, float &y){
    float u = getU(x,y);
    float v = getV(x,y);
    // If X is "between" P and Q
    if ( u > 0 && u < 1) {
      return (v > 0 ? v : (-1 * v));
    }
    // X is closer to P than the segment
    if ( u <= 0 ) {
      Segment PX(P[0], P[1], x, y);
      return PX.PQlength;
    } else { // X is closer to Q than the segment
      Segment QX(Q[0], Q[1], x, y);
      return QX.PQlength;
    }
}

// PS02 - 4.4.1: compute the weight of a segment to a point (x,y) given the weight parameters a,b, and p
float Segment::weight(float &x, float &y, float &a, float &b, float &p){
    float weightLength = pow(PQlength, p);
    float distance = dist(x,y);
    float result = weightLength / (a + distance);
    return pow(result, b);
}

/**************************************************************
 //               DON'T EDIT BELOW THIS LINE                //
 *************************************************************/

// subtracts 2 vectors of the same length.
vector<float> Segment::subtract(vector<float> &vec1, vector<float> &vec2){
// create vector from vec1 to vec2

    vector<float> vec_1_to_2 (vec1.size(), 0);

    if(vec1.size() == vec2.size()){

        for (int i=0; i < (int) vec1.size(); i++){
            vec_1_to_2[i] = vec2[i] - vec1[i];
        }

    }else{
        throw MismatchedSizeException();
    }

    return vec_1_to_2;
}

// takes the dot product between 2 vectors of the same length
float Segment::dot(vector<float> &vec1, vector<float> &vec2){

    float dotProd = 0;

    if(vec1.size() == vec2.size()){

        for (int i=0; i< (int) vec1.size(); i++){
            dotProd += vec2[i]*vec1[i];
        }

    }else{
        throw MismatchedSizeException();
    }

    return dotProd;
}

// mutliplies an entire vector by a scalor value
vector<float> Segment::scalarMult(vector<float> &vec, float &factor){

    vector<float> nVec (vec.size(), 0);
    for(int i=0; i< (int) vec.size(); i++){
        nVec[i] = vec[i]*factor;
    }
    return nVec;
}

// destructor
Segment::~Segment() { } // Nothing to clean up
