#ifndef _Utils_SALOME_Exception_hxx_
#define _Utils_SALOME_Exception_hxx_

# include <exception>
# include <iostream>
#include "SMESH_SMDS.hxx"

#ifdef LOCALIZED
#undef LOCALIZED
#endif

#if defined(_DEBUG_) || defined(_DEBUG)
#define LOCALIZED(message) #message , __FILE__ , __LINE__
#else
#define LOCALIZED(message) #message
#endif

class SALOME_Exception;

SMDS_EXPORT std::ostream& operator<<( std::ostream&, const SALOME_Exception& );
SMDS_EXPORT const char *makeText( const char *text, const char *fileName, const unsigned int lineNumber );

class SMDS_EXPORT SALOME_Exception : public std::exception
{

private :
    SALOME_Exception( void );

protected :
    const char* _text ;

public :
    SALOME_Exception( const char *text, const char *fileName=0, const unsigned int lineNumber=0 );
    SALOME_Exception( const SALOME_Exception &ex );
    virtual ~SALOME_Exception() throw ();
    SMDS_EXPORT friend std::ostream & operator<<( std::ostream &os , const SALOME_Exception &ex );
    virtual const char *what( void ) const throw () ;
} ;

#endif
