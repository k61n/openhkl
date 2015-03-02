#include "Indexer.h"

namespace SX{
	namespace Crystal{


Indexer::Indexer():
		_cell(),
		_dtol(0.02),
		_angletol(0.02)
{

}

Indexer::Indexer(ptrUnitCell cell):
		_cell(cell),
		_dtol(0.02),
		_angletol(0.02)
{

}

Indexer::~Indexer()
{

}

std::vector<Eigen::Vector3i> Indexer::index(const Peak3D& peak)
{
	// First peak, only d-spacing comparison
	if (!_peaks.size())
	{

	}
}


	}
}
