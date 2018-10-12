

#include <cryptxml.h>
#include <memory>
#include <tuple>



class AdES
{
private:

	template <typename T> T* AddMem(std::vector<std::shared_ptr<std::vector<char>>>& mem, size_t sz = sizeof(T))
	{
		shared_ptr<vector<char>> x = make_shared<vector<char>>();
		x->resize(sz);
		mem.push_back(x);
		T* d = (T*)mem[mem.size() - 1].get()->data();
		return d;
	}

	HRESULT GetEncryptedHash(const char*d, DWORD sz, PCCERT_CONTEXT ctx,CRYPT_ALGORITHM_IDENTIFIER hash,std::vector<char> &rs);

public:

	enum class ALEVEL
	{
		S = 0,
		E = 1,
	};

	enum class ATYPE
	{
		CADES = 0,
		XADES = 1
	};


	enum class LEVEL
	{
		CMS = 0,
		XMLDSIG = 0,
		B = 1,
		T = 2,
		C = 3,
		X = 4,
		XL = 5,
		A = 6,
	};

	
	enum class ATTACHTYPE
	{
		DETACHED = 0,
		ENVELOPING = 1,
		ATTACHED = 1,
		ENVELOPED = 2,
	};

	struct SIGNPARAMETERS
	{
		int ConformanceLevel = 0;
		CRYPT_ALGORITHM_IDENTIFIER HashAlgorithm = { szOID_NIST_sha256 };
		ATTACHTYPE Attached = ATTACHTYPE::ATTACHED;
		std::wstring TSServer = L"http://timestamp.comodoca.com/";
		std::string Policy;
		std::string TSPolicy;
		std::string commitmentTypeOid;//		1.2.840.113549.1.9.16.6.1 - 6
		std::vector<CRYPT_ATTRIBUTE> cextras;
		std::string xextras;
		int Type1OrType2 = 2; // For X and XL forms timestamp, currently 2 is supported, this parameter is ignored
		bool ASiC = false; // True if this is for ASiC
		bool Debug = false;
		bool PAdES = false; // True if PAdES, to eliminate self timestamp
		
	};

	struct VERIFYRESULT
	{
		std::string Policy;
		std::string Commitment;
	};
	struct VERIFYRESULTS
	{
		std::vector<VERIFYRESULT> Results;
	};

	struct CERTANDCRL
	{
		PCCERT_CONTEXT cert;
		std::vector<PCCRL_CONTEXT> Crls;
	};
	struct CERT
	{
		CERTANDCRL cert;
		std::vector<CERTANDCRL> More;
	};

	AdES();

	struct FILEREF
	{
		const char* data = 0; // pointer to data
		DWORD sz = 0; // size, or 0 if null terminated XML
		const char* ref = 0;
		std::string mime = "application/octet-stream";

		FILEREF(const char * d = 0, DWORD z = 0, const char* rref = 0, const char* mim = 0)
		{
			data = d;
			sz = z;
			ref = rref;
			if (mim)
				mime = mim;
		}
	};

	HRESULT AddCT(std::vector<char>& Signature, const std::vector<CERT>& Certificates, SIGNPARAMETERS& Params);
	std::tuple<HRESULT,std::vector<char>,std::vector<char>> AddCC(std::vector<char>& Signature, const std::vector<CERT>& Certificates,SIGNPARAMETERS& Params);
	HRESULT AddCX(std::vector<char>& Signature, const std::vector<CERT>& Certificates, SIGNPARAMETERS& Params, std::vector<char>& full1, std::vector<char >&full2);
	HRESULT AddCXL(std::vector<char>& Signature, const std::vector<CERT>& Certificates, SIGNPARAMETERS& Params);

	HRESULT TimeStamp(SIGNPARAMETERS& params,const char* data, DWORD sz, std::vector<char>& CR, const wchar_t* url = L"http://timestamp.comodoca.com/", const char* alg = szOID_NIST_sha256);
	HRESULT Sign(LEVEL lev,const char* data,DWORD sz,const std::vector<CERT>& Certificates, SIGNPARAMETERS& Params,std::vector<char>& Signature);
	HRESULT Verify(const char* data, DWORD sz, LEVEL& lev,const char* omsg = 0,DWORD len = 0,std::vector<char>* msg = 0,std::vector<PCCERT_CONTEXT>* Certs = 0,VERIFYRESULTS* vr = 0);
	HRESULT VerifyB(const char* data, DWORD sz, int sidx = 0,bool Attached = true,PCCERT_CONTEXT c = 0);
	HRESULT VerifyT(const char* data, DWORD sz, PCCERT_CONTEXT* pX = 0, bool Attached = true, int TSServerSignIndex = 0, FILETIME* ft = 0);
	HRESULT VerifyU(const char* data, DWORD sz, bool Attached = true, int TSServerSignIndex = 0);
	HRESULT XMLSign(LEVEL lev, std::vector<FILEREF>& data,const std::vector<CERT>& Certificates,SIGNPARAMETERS& Params, std::vector<char>& Signature);

	HRESULT PDFSign(LEVEL lev, const char* data, DWORD sz, const std::vector<CERT>& Certificates, SIGNPARAMETERS& Params, std::vector<char>& Signature);

	HRESULT ASiC(ALEVEL alev,ATYPE typ, LEVEL lev,std::vector<FILEREF>& data,std::vector<CERT>& Certificates, SIGNPARAMETERS& Params, std::vector<char>& fndata);

};

