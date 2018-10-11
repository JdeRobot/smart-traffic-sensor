/**
 * @file       singleton.h
 * @brief      Implementation of the CSingleton template class.
 * @author     Brian van der Beek
 */

#ifndef __SINGLETON_H__
#define __SINGLETON_H__

/* The CSingleton class is a template class for creating singleton objects.
 * When the static Instance() method is called for the first time, the singleton
 * object is created. Every sequential call returns a reference to this instance.
 * The class instance can be destroyed by calling the DestroyInstance() method.
 */
template <typename T>
class CSingleton
{
public:

  /* Gets a reference to the instance of the singleton class.
   *
   * return A reference to the instance of the singleton class.
   * If there is no instance of the class yet, one will be created.
   */
  static T* Instance()
  {
    if (m_instance == NULL) m_instance = new T;
    return m_instance;
  };

  /* Destroys the singleton class instance.
   *
   * Be aware that all references to the single class instance will be
   * invalid after this method has been executed!
   */
  static void DestroyInstance()
  {
    delete m_instance;
    m_instance = NULL;
  };

protected:

  /* shield the constructor and destructor to prevent outside sources
   * from creating or destroying a CSingleton instance.
   */
  CSingleton(){};

  /* Copy constructor.*/
  CSingleton(const CSingleton& source){};

  /* Destructor.*/
  virtual ~CSingleton(){printf("Destroying instance ..");};

private:

  /*singleton class instance*/
  static T* m_instance;
};

/* static class member initialisation.*/
template <typename T> T* CSingleton<T>::m_instance = NULL;

#endif
