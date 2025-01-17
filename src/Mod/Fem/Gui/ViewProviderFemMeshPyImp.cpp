
#include "PreCompiled.h"

#include <Base/VectorPy.h>
#include <Base/GeometryPyCXX.h>

#include <App/DocumentObjectPy.h>

#include "Mod/Fem/Gui/ViewProviderFemMesh.h"
#include "Mod/Fem/App/FemResultObject.h"

// inclusion of the generated files (generated out of ViewProviderFemMeshPy.xml)
#include "ViewProviderFemMeshPy.h"
#include "ViewProviderFemMeshPy.cpp"

using namespace FemGui;

// returns a string which represents the object e.g. when printed in python
std::string ViewProviderFemMeshPy::representation(void) const
{
    return std::string("<ViewProviderFemMesh object>");
}



PyObject* ViewProviderFemMeshPy::applyDisplacement(PyObject * args)
{
    double factor;
    if (!PyArg_ParseTuple(args, "d", &factor))
        return 0;

    this->getViewProviderFemMeshPtr()->applyDisplacementToNodes(factor);

    Py_Return;
}

App::Color calcColor(double value,double min, double max)
{
    if (max < 0) max = 0;
    if (min > 0) min = 0;

    if (value < min) 
        return App::Color (0.0,0.0,1.0);    
    if (value > max)
        return App::Color (1.0,0.0,0.0);
    if (value == 0.0)
        return App::Color (0.0,1.0,0.0);
    if ( value > max/2.0 )
        return App::Color (1.0,1-((value-(max/2.0)) / (max/2.0)),0.0);
    if ( value > 0.0 )
        return App::Color (value/(max/2.0),1.0,0.0) ;
    if ( value < min/2.0 )
        return App::Color (0.0,1-((value-(min/2.0)) / (min/2.0)),1.0);
    if ( value < 0.0 )
        return App::Color (0.0,1.0,value/(min/2.0)) ;
    return App::Color (0,0,0);
}


PyObject* ViewProviderFemMeshPy::setNodeColorByScalars(PyObject *args)
{
    double max = -1e12;
    double min = +1e12;
    PyObject *node_ids_py;
    PyObject *values_py;

    if (PyArg_ParseTuple(args,"O!O!",&PyList_Type, &node_ids_py, &PyList_Type, &values_py)) {
        std::vector<long> ids;
        std::vector<double> values;
        int num_items = PyList_Size(node_ids_py);
        if (num_items < 0) {
            PyErr_SetString(Base::BaseExceptionFreeCADError, "PyList_Size < 0. That is not a valid list!");
            Py_Return;
        }
        std::vector<App::Color> node_colors(num_items);
        for (int i=0; i<num_items; i++){
            PyObject *id_py = PyList_GetItem(node_ids_py, i);
            long id = PyLong_AsLong(id_py);
            ids.push_back(id);
            PyObject *value_py = PyList_GetItem(values_py, i);
            double val = PyFloat_AsDouble(value_py);
            values.push_back(val);
            if(val > max)
                max = val;
            if(val < min)
                min = val;
        }
        long i=0;
        for(std::vector<double>::const_iterator it=values.begin(); it!=values.end(); ++it, i++)
            node_colors[i] = calcColor(*it, min, max);
        this->getViewProviderFemMeshPtr()->setColorByNodeId(ids, node_colors);
    } else {
        PyErr_SetString(Base::BaseExceptionFreeCADError, "PyArg_ParseTuple failed. Invalid arguments used with setNodeByScalars");
    }
    Py_Return;
}


PyObject* ViewProviderFemMeshPy::setNodeDisplacementByVectors(PyObject *args)
{
    PyObject *node_ids_py;
    PyObject *vectors_py;
    if (PyArg_ParseTuple(args,"O!O!",&PyList_Type, &node_ids_py, &PyList_Type, &vectors_py)) {
        std::vector<long> ids;
        std::vector<Base::Vector3d> vectors;
        int num_items = PyList_Size(node_ids_py);
        if (num_items < 0) {
            PyErr_SetString(Base::BaseExceptionFreeCADError, "PyList_Size < 0. That is not a valid list!");
            Py_Return;
        }
        for (int i=0; i<num_items; i++){
            PyObject *id_py = PyList_GetItem(node_ids_py, i);
            long id = PyLong_AsLong(id_py);
            ids.push_back(id);
            PyObject *vector_py = PyList_GetItem(vectors_py, i);
            Base::Vector3d vec = Base::getVectorFromTuple<double>(vector_py);
            vectors.push_back(vec);
        }
        this->getViewProviderFemMeshPtr()->setDisplacementByNodeId(ids, vectors);
    } else {
        PyErr_SetString(Base::BaseExceptionFreeCADError, "PyArg_ParseTuple failed. Invalid arguments used with setNodeDisplacementByVectors");
    }
    Py_Return;
}

Py::Dict ViewProviderFemMeshPy::getNodeColor(void) const
{
    //return Py::List();
    throw Py::AttributeError("Not yet implemented");
}

void ViewProviderFemMeshPy::setNodeColor(Py::Dict arg)
{
    long size = arg.size();
    if(size == 0)
        this->getViewProviderFemMeshPtr()->resetColorByNodeId();
    else {
        Base::TimeInfo Start;
        Base::Console().Log("Start: ViewProviderFemMeshPy::setNodeColor() =================================\n");
        //std::map<long,App::Color> NodeColorMap;

        //for( Py::Dict::iterator it = arg.begin(); it!= arg.end();++it){
        //    Py::Int id((*it).first);
        //    Py::Tuple color((*it).second);
        //    NodeColorMap[id] = App::Color(Py::Float(color[0]),Py::Float(color[1]),Py::Float(color[2]),0);
        //}
        std::vector<long> NodeIds(size);
        std::vector<App::Color> NodeColors(size);

        long i = 0;
        for( Py::Dict::iterator it = arg.begin(); it!= arg.end();++it,i++){
            Py::Int id((*it).first);
            Py::Tuple color((*it).second);
            NodeIds[i]    = id;
            NodeColors[i] = App::Color(Py::Float(color[0]),Py::Float(color[1]),Py::Float(color[2]),0);
        }
        Base::Console().Log("    %f: Start ViewProviderFemMeshPy::setNodeColor() call \n",Base::TimeInfo::diffTimeF(Start,Base::TimeInfo()));

        //this->getViewProviderFemMeshPtr()->setColorByNodeId(NodeColorMap);
        this->getViewProviderFemMeshPtr()->setColorByNodeId(NodeIds,NodeColors);
        Base::Console().Log("    %f: Finish ViewProviderFemMeshPy::setNodeColor() call \n",Base::TimeInfo::diffTimeF(Start,Base::TimeInfo()));
	}
}

Py::Dict ViewProviderFemMeshPy::getElementColor(void) const
{
    //return Py::List();
    throw Py::AttributeError("Not yet implemented");
}

void ViewProviderFemMeshPy::setElementColor(Py::Dict arg)
{
    if(arg.size() == 0)
        this->getViewProviderFemMeshPtr()->resetColorByNodeId();
    else {
        std::map<long,App::Color> NodeColorMap;

        for( Py::Dict::iterator it = arg.begin(); it!= arg.end();++it){
            Py::Int id((*it).first);
            Py::Tuple color((*it).second);
            NodeColorMap[id] = App::Color(Py::Float(color[0]),Py::Float(color[1]),Py::Float(color[2]),0);
        }
        this->getViewProviderFemMeshPtr()->setColorByElementId(NodeColorMap);
	}
}

Py::Dict ViewProviderFemMeshPy::getNodeDisplacement(void) const
{
    //return Py::Dict();
    throw Py::AttributeError("Not yet implemented");
}

void  ViewProviderFemMeshPy::setNodeDisplacement(Py::Dict arg)
{
    if(arg.size() == 0)
        this->getViewProviderFemMeshPtr()->resetColorByNodeId();
    else {
        std::map<long,Base::Vector3d> NodeDispMap;
        union PyType_Object pyType = {&(Base::VectorPy::Type)};
        Py::Type vType(pyType.o);

        for( Py::Dict::iterator it = arg.begin(); it!= arg.end();++it){
            Py::Int id((*it).first);
            if ((*it).second.isType(vType)) {
                Py::Vector p((*it).second);
                NodeDispMap[id] = p.toVector();
            }
        }
        this->getViewProviderFemMeshPtr()->setDisplacementByNodeId(NodeDispMap);
	}
}

Py::List ViewProviderFemMeshPy::getHighlightedNodes(void) const
{
    //return Py::List();
    throw Py::AttributeError("Not yet implemented");
}

void  ViewProviderFemMeshPy::setHighlightedNodes(Py::List arg)
{
    std::set<long> res;

    for( Py::List::iterator it = arg.begin(); it!= arg.end();++it){
        Py::Int id(*it);
        if(id)
            res.insert(id);
    }
    this->getViewProviderFemMeshPtr()->setHighlightNodes(res);
}

Py::List ViewProviderFemMeshPy::getVisibleElementFaces(void) const
{
    const std::vector<unsigned long> & visElmFc = this->getViewProviderFemMeshPtr()->getVisibleElementFaces();
    std::vector<unsigned long> trans;

    // sorting out double faces through higer order elements and null entries
    long elementOld =0, faceOld=0;
    for (std::vector<unsigned long>::const_iterator it = visElmFc.begin();it!=visElmFc.end();++it){
        if(*it == 0)
            continue;

        long element = *it>>3;
        long face    = (*it&7)+1;
        if(element == elementOld && face==faceOld)
            continue;

        trans.push_back(*it);
        elementOld = element;
        faceOld    = face;
    }

    Py::List result( trans.size() );
    int i = 0;
    for (std::vector<unsigned long>::const_iterator it = trans.begin();it!=trans.end();++it,i++){
        Py::Tuple tup(2);
        long element = *it>>3;
        long face    = (*it&7)+1;
        tup.setItem( 0,Py::Int( element ) );
        tup.setItem( 1,Py::Int( face ) );
        result.setItem(i,tup);
    }

    return result;
}


PyObject *ViewProviderFemMeshPy::getCustomAttributes(const char* /*attr*/) const
{
    return 0;
}

int ViewProviderFemMeshPy::setCustomAttributes(const char* /*attr*/, PyObject* /*obj*/)
{
    return 0; 
}


