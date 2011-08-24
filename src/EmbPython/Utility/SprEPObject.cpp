#include "..\..\..\include\EmbPython\Utility\SprEPObject.h"



/////////////////////////////////////���\�b�h�o�^�p
///void�̃��\�b�h�̒�`
static PyMethodDef EPObject_methods[] =
{
	{NULL}
};

/////////////////////////////////////////�����o�ϐ��o�^�p
/*������*/

//////////////////////////////////////////init
static int
EPObject_init(EPObject* self,PyObject *args, PyObject *kwds)
{
	return 0;
}
//////////////////////////////////////////new
static PyObject*
EPObject_new(PyTypeObject *type,PyObject *args, PyObject *kwds)
{
	EPObject *self;
	self = ( EPObject*) type->tp_alloc(type,0);
	if ( self != NULL)
	{
		EPObject_init(self,args,kwds);
	}
	return (PyObject *)self;
}
////////////////////////////////////////////dealloc
static void
EPObject_dealloc(EPObject* self)
{
	//Python�ł���Ȃ��Ă�Springhead�Ŏg���Ă邩������Ȃ�
	//delete(self->ptr);
	self->ob_base.ob_type->tp_free((PyObject*)self);
}

PyTypeObject EPObjectType =
{
	PyVarObject_HEAD_INIT(NULL,0)
	"Test.void",             /*tp_name*/
	sizeof(EPObject),             /*tp_basicsize*/
	0,                         /*tp_itemsize*/
	(destructor)EPObject_dealloc,                        /*tp_dealloc*/
	0,                         /*tp_print*/
	0,                         /*tp_getattr*/
	0,                         /*tp_setattr*/
	0,                         /*tp_compare*/
	0,                         /*tp_repr*/
	0,                         /*tp_as_number*/
	0,                         /*tp_as_sequence*/
	0,                         /*tp_as_mapping*/
	0,                         /*tp_hash */
	0,                         /*tp_call*/
	0,                         /*tp_str*/
	0,                         /*tp_getattro*/
	0,                         /*tp_setattro*/
	0,                         /*tp_as_buffer*/
	Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE , /*tp_flags*/
	"void",           /* tp_doc */
	0,		               /* tp_traverse */
	0,		               /* tp_clear */
	0,		               /* tp_richcompare */
	0,		               /* tp_weaklistoffset */
	0,		               /* tp_iter */
	0,		               /* tp_iternext */
	EPObject_methods,             /* tp_methods */
	0,//EPObject_members,             /* tp_members */
	0,                         /* tp_getset */
	0,                         /* tp_base */
	0,                         /* tp_dict */
	0,                         /* tp_descr_get */
	0,                         /* tp_descr_set */
	0,                         /* tp_dictoffset */
	(initproc)EPObject_init,      /* tp_init */
	0,                         /* tp_alloc */
	(newfunc)EPObject_new,                 /* tp_new */

};

PyMODINIT_FUNC initEPObject(void)
{
	PyObject* m;

	//Python�N���X�̍쐬
	if ( PyType_Ready( &EPObjectType ) < 0 ) return NULL ;

	m = PyImport_AddModule("Utility");

	//���W���[���ɒǉ�
	Py_INCREF(&EPObjectType);
	PyModule_AddObject(m,"void",(PyObject *)&EPObjectType);
	return m;
}

EPObject* newEPObject()
{
	return (EPObject*)EPObject_new(&EPObjectType,NULL,NULL);
}

EPObject* newEPObject(const void* ptr)
{
	EPObject* ret = (EPObject*)EPObject_new(&EPObjectType,NULL,NULL);
	ret->ptr = ptr;

	return ret;
}