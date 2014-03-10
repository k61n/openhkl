#include "BlobFinder.h"

namespace SX
{

namespace Geometry
{

	void registerEquivalence(int a, int b, vipairs& equivalences)
	{
		if (a < b)
			equivalences.push_back(vipairs::value_type(b,a));
		else
			equivalences.push_back(vipairs::value_type(a,b));
	}

	bool sortEquivalences(const ipair& pa, const ipair& pb)
	{
		if (pa.first<pb.first)
			return true;
		if (pa.first>pb.first)
			return false;
		return (pa.second<pb.second);
	}

	imap removeDuplicates(vipairs& equivalences)
	{
		auto beg=equivalences.begin();
		auto last=std::unique(equivalences.begin(),equivalences.end());

		imap mequiv;
		for (auto it=beg;it!=last;++it)
			mequiv.insert(*it);

		return mequiv;
	}

	void reassignEquivalences(imap& equivalences)
	{
		for (auto it=equivalences.begin();it!=equivalences.end();++it)
		{
			auto found=equivalences.find(it->second);
			if (found!=equivalences.end())
				it->second=found->second;
		}
	}

} // namespace Geometry

} // namespace SX
