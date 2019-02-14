#include <string>
#include <vector>

#include <core/NSXTest.h>
#include <core/ProgressHandler.h>

NSX_INIT_TEST

using namespace nsx;

int main() {
  ProgressHandler handler;

  // check that it handles nullptr without exception
  handler.setCallback(nullptr);

  // send it a dummy callback
  int count = 0;
  handler.setCallback([&count]() { ++count; });

  handler.setProgress(50);
  NSX_CHECK_ASSERT(handler.getProgress() == 50);
  NSX_CHECK_ASSERT(count == 1); // modified by callback

  handler.setStatus("progress handler test");
  NSX_CHECK_ASSERT(handler.getStatus() == "progress handler test");

  handler.log("progress handler log 1");
  handler.log("progress handler log 2");
  std::vector<std::string> log = handler.getLog();
  NSX_CHECK_ASSERT(log.size() == 2);
  NSX_CHECK_ASSERT(log[0] == "progress handler log 1");
  NSX_CHECK_ASSERT(log[1] == "progress handler log 2");

  NSX_CHECK_ASSERT(handler.getLog().size() == 0);

  handler.log(std::string("string log"));
  NSX_CHECK_ASSERT(handler.getLog()[0] == "string log");

  handler.abort();
  NSX_CHECK_ASSERT(handler.aborted() == true);

  // check correct behaviour under abort
  bool aborted = false;

  try {
    handler.setProgress(20);
  } catch (std::exception &e) {
    aborted = true;
  }
  NSX_CHECK_ASSERT(aborted == true);

  return 0;
}
