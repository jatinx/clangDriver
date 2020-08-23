#include "helper.hh"
#include <iostream>

using namespace llvm;
using namespace llvm::opt;
using namespace clang;
using namespace clang::driver;

std::string LogStr;
raw_string_ostream LogS_(LogStr);
llvm::raw_ostream &LogS = LogS_;

int main() {
  initializeLLVM();
  SmallVector<const char *, 128> Argv;

  Argv.push_back("");
  Argv.push_back("-target");
  Argv.push_back("x86_64-pc-linux-gnu");
  Argv.push_back("/tmp/jatin.cc");
  Argv.push_back("-o");
  Argv.push_back("/tmp/jatin.o");
  Argv.push_back("-S");

  ArrayRef<const char *> Args = Argv; // Coz why tf not

  // Add clang driver option
  IntrusiveRefCntPtr<DiagnosticOptions> DiagOpts(new DiagnosticOptions);
  unsigned MissingArgIndex, MissingArgCount;
  InputArgList ArgList = getDriverOptTable().ParseArgs(
      Args.slice(1), MissingArgIndex, MissingArgCount);
  (void)ParseDiagnosticArgs(*DiagOpts, ArgList);
  TextDiagnosticPrinter *DiagClient =
      new TextDiagnosticPrinter(LogS, &*DiagOpts);
  IntrusiveRefCntPtr<DiagnosticIDs> DiagID(new DiagnosticIDs);
  DiagnosticsEngine Diags(DiagID, &*DiagOpts, DiagClient);
  ProcessWarningOptions(Diags, *DiagOpts, /*ReportDiags=*/false);

  Driver TheDriver("", "", Diags);
  TheDriver.setTitle("Custom code object manager");
  TheDriver.setCheckInputsExist(false);

  std::unique_ptr<Compilation> C(TheDriver.BuildCompilation(Args));
  if (!C) {
    if (C->containsError()) {
      std::cout << "Compilation contains errors:: " << LogStr << std::endl;
    }
    return 0;
  }
  for (auto &Job : C->getJobs()) {
    auto Arguments = Job.getArguments();
    // auto term = '\n';
    // Job.Print(LogS, &term, false);
    SmallVector<const char *, 128> argv;
    argv.clear();
    // argv.push_back("");
    argv.append(Arguments.begin(), Arguments.end());
    argv.push_back(nullptr);

    std::unique_ptr<CompilerInstance> Clang(new CompilerInstance());
    Clang->setVerboseOutputStream(LogS);
    if (!CompilerInvocation::CreateFromArgs(Clang->getInvocation(), argv,
                                            Diags)) {
      std::cout << "Error creating compiler invocation:: " << LogStr
                << std::endl;
      return 0;
    }
    Clang->createDiagnostics(DiagClient, /* ShouldOwnClient */ false);
    if (!Clang->hasDiagnostics()) {
      std::cout << "Error creating compiler diag:: " << LogStr << std::endl;
      return 0;
    }
    if (!ExecuteCompilerInvocation(Clang.get())) {
      std::cout << "Error exec compiler invocation:: " << LogStr << std::endl;
      return 0;
    }
  }
  std::cout << "Fin:: " << LogStr << std::endl;
}