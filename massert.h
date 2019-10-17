#ifndef MASSERT_H
#define MASSERT_H

#define MASSERT(Expression) {               \
    try{                                                     \
        CV_Assert(Expression);                   \
    }catch(cv::Exception ex){                    \
        throw std::exception(ex.what());     \
    }                                                       \
}

#endif // MASSERT_H
