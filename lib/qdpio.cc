// $Id: qdpio.cc,v 1.9 2003-05-12 06:08:41 edwards Exp $
//
/*! @file
 * @brief IO support via QIO
 */

#include "qdp.h"

QDP_BEGIN_NAMESPACE(QDP);

//-----------------------------------------
static int get_node_number(const int coord[])
{
  multi1d<int> crd(Nd);
  crd = coord;   // an array copy
  int node = Layout::nodeNumber(crd);
  return node;
}


//-----------------------------------------
// QDP QIO support
QDPSerialReader::QDPSerialReader() {iop=false;}

QDPSerialReader::QDPSerialReader(XMLReader& xml, const std::string& p) {open(xml,p);}

void QDPSerialReader::open(XMLReader& file_xml, const std::string& path) 
{
  QIO_Layout *layout = new QIO_Layout;
  int latsize[Nd];

  for(int m=0; m < Nd; ++m)
    latsize[m] = Layout::lattSize()[m];

  layout->node_number = &get_node_number;
  layout->latsize = latsize; // local copy
  layout->latdim = Nd; 
  layout->volume = Layout::vol(); 
  layout->this_node = Layout::nodeNumber(); 

  // Initialize string objects 
  XML_string *xml_c  = XML_string_create(0);

  if ((qio_in = QIO_open_read(xml_c, path.c_str(), QIO_SERIAL, layout)) == NULL)
    QDP_error_exit("QDPSerial::Reader: failed to open file %s",path.c_str());

  // Use string to initialize XMLReader
  istringstream ss((const string)(XML_string_ptr(xml_c)));
  file_xml.open(ss);

  XML_string_destroy(xml_c);
  delete layout;

  iop=true;
}

void QDPSerialReader::close()
{
  if (is_open()) 
  {
    int status = QIO_close_read(qio_in);
  }

  iop = false;
}

bool QDPSerialReader::is_open() {return iop;}

bool QDPSerialReader::eof() const {return false;}

bool QDPSerialReader::bad() const {return false;}

QDPSerialReader::~QDPSerialReader() {close();}


//-----------------------------------------
//! text writer support
QDPSerialWriter::QDPSerialWriter() {iop=false;}

QDPSerialWriter::QDPSerialWriter(const XMLMetaWriter& xml, const std::string& p) 
{
  open(xml,p);
}

void QDPSerialWriter::open(const XMLMetaWriter& file_xml, const std::string& path) 
{
  QIO_Layout *layout = new QIO_Layout;
  int latsize[Nd];

  for(int m=0; m < Nd; ++m)
    latsize[m] = Layout::lattSize()[m];

  layout->node_number = &get_node_number;
  layout->latsize = latsize; // local copy
  layout->latdim = Nd; 
  layout->volume = Layout::vol(); 
  layout->this_node = Layout::nodeNumber(); 

  // Copy metadata string into simple qio string container
  XML_string* xml_c = XML_string_create(file_xml.str().length()+1);  // check if +1 is needed
  XML_string_set(xml_c, file_xml.str().c_str());

  // Big call to qio
  if ((qio_out = QIO_open_write(xml_c, path.c_str(), QIO_SERIAL, QIO_LEX_ORDER, QIO_CREATE, 
				layout)) == NULL)
    QDP_error_exit("QDPSerial::Writer: failed to open file %s",path.c_str());

  // Cleanup
  XML_string_destroy(xml_c);
  delete layout;

  iop=true;
}

void QDPSerialWriter::close()
{
  if (is_open()) 
  {
    int status = QIO_close_write(qio_out);
  }

  iop = false;
}

bool QDPSerialWriter::is_open() {return iop;}

bool QDPSerialWriter::bad() const {return false;}

QDPSerialWriter::~QDPSerialWriter() {close();}


QDP_END_NAMESPACE();
