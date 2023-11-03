void test_read2()
{
    auto file = new TFile("data/texat_0824.raw.root");
    auto tree = file -> Get<TTree>("event");
    auto list = tree -> GetListOfBranches();
    list -> Print();
    auto numBranches = list -> GetEntries();
    for (auto iBranch=0; iBranch<numBranches; ++iBranch) {
        auto branch = (TBranch *) list -> At(iBranch);
        cout << branch -> GetName() << " " << branch -> GetClassName() << endl;
    }
}

