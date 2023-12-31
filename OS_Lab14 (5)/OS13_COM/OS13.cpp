#include "pch.h"
#include "ICom.h"
#include "SEQLOG.h"
#include "OS11_HTAPI.h"

extern ULONG g_Components;

OS13::OS13() :m_lRef(1) {
	SEQ;
	InterlockedIncrement((LONG*)&g_Components);
	LOG("OS13::Adder g_Components = ", g_Components);
};

OS13::~OS13() {
	SEQ;
	InterlockedDecrement((LONG*)&g_Components);
	LOG("OS13::~Adder g_Components = ", g_Components);
};

STDMETHODIMP OS13::QueryInterface(REFIID riid, void** ppv)
{
	SEQ;
	HRESULT rc = S_OK;
	*ppv = NULL;
	if (riid == IID_IUnknown)
		*ppv = (IOS13*)this;
	else if (riid == IID_IOS14)
		*ppv = (IOS13*)this;
	else rc = E_NOINTERFACE;

	if (rc == S_OK) this->AddRef();
	LOG("OS13::QueryInterface rc = ", rc);
	return rc;
};

STDMETHODIMP_(ULONG) OS13::AddRef(void) {
	SEQ;
	InterlockedIncrement((LONG*)&(this->m_lRef));
	LOG("OS13::AddRef m_lRef = ", this->m_lRef);
	return this->m_lRef;
};
STDMETHODIMP_(ULONG) OS13::Release(void) {
	SEQ;
	ULONG rc = this->m_lRef;
	if ((rc = InterlockedDecrement((LONG*)&(this->m_lRef))) == 0) delete this;
	LOG("OS13::Release rc = ", rc);
	return rc;
};

STDMETHODIMP OS13::Add(const double lOp1, const double lOp2, double* pResult)
{
	*pResult = lOp1 + lOp2;
	return S_OK;
}

STDMETHODIMP OS13::Sub(const double lOp1, const double lOp2, double* pResult)
{
	*pResult = lOp1 - lOp2;
	return S_OK;
}

STDMETHODIMP OS13::Create(
	int	  Capacity,					   // ������� ���������
	int   SecSnapshotInterval,		   // ������������� ���������� � ���.
	int   MaxKeyLength,                // ������������ ������ �����
	int   MaxPayloadLength,            // ������������ ������ ������
	const wchar_t  FileName[512],      // ��� ����� 
	const wchar_t  HTUsersGroup[512],      // ��� �����  
	HT::HTHANDLE** result
){
	*result = HT::Create(Capacity, SecSnapshotInterval, MaxKeyLength, MaxPayloadLength, FileName, HTUsersGroup);
	if (*result == NULL) {
		return E_FAIL;
	}
	return S_OK;
}

STDMETHODIMP OS13::Create(
	int	  Capacity,					   // ������� ���������
	int   SecSnapshotInterval,		   // ������������� ���������� � ���.
	int   MaxKeyLength,                // ������������ ������ �����
	int   MaxPayloadLength,            // ������������ ������ ������
	const wchar_t  FileName[512],      // ��� ����� 
	const wchar_t  HTUsersGroup[512],      // ��� �����  
	const wchar_t userName[512],
	const wchar_t password[512],
	HT::HTHANDLE** result
){
	*result = HT::Create(Capacity, SecSnapshotInterval, MaxKeyLength, MaxPayloadLength, FileName, HTUsersGroup, userName, password);
	if (*result == NULL) {
		return E_FAIL;
	}
	return S_OK;
}

STDMETHODIMP OS13::Open(
	const wchar_t    FileName[512],         // ��� ����� 
	HT::HTHANDLE** result
) {
	HT::HTHANDLE* temp = HT::Open(FileName);
	if (temp == NULL)
		return E_FAIL;
	*result = temp;
	if (*result != NULL)
		return S_OK;
	else
		return E_FAIL;
}
STDMETHODIMP OS13::Open(
	const wchar_t    FileName[512],         // ��� ����� 
	const wchar_t    userName[512],         // ��� ����� 
	const wchar_t    password[512],         // ��� ����� 
	HT::HTHANDLE** result
) {
	HT::HTHANDLE* temp = HT::Open(FileName, userName, password);
	if (temp == NULL)
		return E_FAIL;
	*result = temp;
	if (*result != NULL)
		return S_OK;
	else
		return E_FAIL;
}

STDMETHODIMP OS13::Start(
	const wchar_t    FileName[512],         // ��� ����� 
	const wchar_t    userName[512],         // ��� ����� 
	const wchar_t    password[512],         // ��� ����� 
	HT::HTHANDLE** result
) {
	HT::HTHANDLE* temp = HT::Start(FileName, userName, password);
	if (temp == NULL)
		return E_FAIL;
	*result = temp;
	if (*result != NULL)
		return S_OK;
	else
		return E_FAIL;
}

STDMETHODIMP OS13::Start(
	const wchar_t    FileName[512],         // ��� ����� 
	HT::HTHANDLE** result
) {
	HT::HTHANDLE* temp = HT::Start(FileName);
	if (temp == NULL)
		return E_FAIL;
	*result = temp;
	if (*result != NULL)
		return S_OK;
	else
		return E_FAIL;
}

STDMETHODIMP OS13::Close(
	HT::HTHANDLE** hthandle
) {
	bool isSuc =  HT::Close(*hthandle);
	if (isSuc)
		return S_OK;
	else
		return E_FAIL;
}

STDMETHODIMP OS13::Snap(
	HT::HTHANDLE** hthandle
) {
	if (HT::Snap(*hthandle))
		return S_OK;
	else
		return E_FAIL;
}

STDMETHODIMP OS13::Insert(
	HT::HTHANDLE** hthandle,            // ���������� HT
	HT::Element** element              // �������
) {
	if (HT::Insert(*hthandle, *element))
		return S_OK;
	else
		return E_FAIL;
}

STDMETHODIMP OS13::Get(
	HT::Element** result,
	HT::HTHANDLE** hthandle,            // ���������� HT
	HT::Element** element              // �������
) {
	HT::Element* temp = HT::Get(*hthandle, *element);
	if (temp == NULL)
		return E_FAIL;
	*result = temp;
	return S_OK;
}

STDMETHODIMP OS13::Delete(
	HT::HTHANDLE** hthandle,            // ���������� HT (����)
	HT::Element** element              // ������� 
) {
	if (HT::Delete(*hthandle, *element))
		return S_OK;
	else
		return E_FAIL;
}

STDMETHODIMP OS13::Update(
	HT::HTHANDLE** hthandle,            // ���������� HT
	HT::Element** oldelement,          // ������ ������� (����, ������ �����)
	const char** newpayload,          // ����� ������  
	int*             newpayloadlength     // ������ ����� ������
) {
	if (HT::Update(*hthandle, *oldelement, *newpayload, *newpayloadlength))
		return S_OK;
	else
		return E_FAIL;
}

STDMETHODIMP OS13::GetLastErr(
	std::string& error,
	HT::HTHANDLE** ht                         // ���������� HT
) {
	error = std::string(HT::GetLastErr(*ht));
	return S_OK;
}

STDMETHODIMP OS13::CreateElement(
	void* key,
	void* payload,
	HT::Element** el			                  // ���������� HT
) {
	*el = new HT::Element(key, strlen((const char*)key), payload, strlen((const char*)payload));
	if (*el == NULL)
		return E_FAIL;
	return S_OK;
	
}

STDMETHODIMP OS13::Print(
	HT::Element** element              // ������� 
) {
	HT::Print(*element);
	return S_OK;
}