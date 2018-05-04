#include <iostream>

#include <nsxlib/ResourcesFactory.h>

int main()
{
    nsx::ResourcesFactory factory;

    auto& res = factory.create("instruments");
    for (auto r : res) {
        std::cout<<r.first<<std::endl;
    }
}
