#ifndef m_stats_LinkDef_h
#define m_stats_LinkDef_h

#ifdef __CINT__

// MSConfig.h
#pragma link C++ class mst::MSConfig-;
#pragma link C++ typedef mst::MSConfigSubSetPair;
#pragma link C++ typedef mst::MSConfigSubSetMap;
#pragma link C++ typedef mst::MSConfigPair;
#pragma link C++ typedef mst::MSConfigMap;

// MSDataObject.h:
#pragma link C++ class mst::MSDataObject-;

// MSDataPoint.h:
#pragma link C++ class mst::MSDataPoint-;
#pragma link C++ typedef mst::MSDataPointPair;
#pragma link C++ typedef mst::MSDataPointMap;
#pragma link C++ typedef mst::MSDataPointVector;

// MSDataSet.h
#pragma link C++ class mst::MSDataSet-;

// MSMinimizer.h
#pragma link C++ class mst::MSMinimizer-;

// MSModel.h
#pragma link C++ class mst::MSModel-;
#pragma link C++ typedef mst::MSModelVector

// MSParameter.h
#pragma link C++ class mst::MSParameter-;
// #pragma link C++ typedef mst::MSParameterVector
#pragma link C++ typedef mst::MSParameterMap
#pragma link C++ typedef mst::MSParameterPair

#endif // __CINT__

#endif // m_stats_LinkDef_h
