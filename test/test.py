import pathlib
import asyncio
from rich import print

#Path to the reference implementation
refPath = "craftinginterpreters/clox"
totaltests= 0
passedtests = 0
failedtests = 0
#Path to the test implementation
testPath = "../build/clox"
debug = False
# Find all files with .lox extension in the test directory
testFiles = []

# Run each task in an async cluster
async def run_test_async(file):
    global totaltests, passedtests, failedtests
    totaltests +=1
    proc_ref = await asyncio.create_subprocess_exec(refPath, file, stdout=asyncio.subprocess.PIPE,stderr=asyncio.subprocess.PIPE)
    proc_test = await asyncio.create_subprocess_exec(testPath, file, stdout=asyncio.subprocess.PIPE,stderr=asyncio.subprocess.PIPE)
    try:
        async with asyncio.timeout(10):
            proc_ref_out, proc_ref_err = await proc_ref.communicate()
            proc_test_out, proc_test_err = await proc_test.communicate()
            testStatus = (proc_ref_out == proc_test_out) and (proc_ref_err == proc_test_err) and (proc_ref.returncode == proc_test.returncode)
        if testStatus:
            print(f"[bold]{file}[/bold] [green]Test Passed:[/green]")
            passedtests +=1
        else :
            print(f"[bold]{file}[/bold] [red]Test Failed:[/red]")
            failedtests +=1
            if debug:
                print("Reference: " + proc_ref_out.decode('utf-8'))
                print("Test: " + proc_test_out.decode('utf-8'))
                print("Reference Error: " + proc_ref_err.decode('utf-8'))
                print("Test Error: " + proc_test_err.decode('utf-8'))
                print("Test Status: " + str(proc_test.returncode))
                print("Reference Status: " + str(proc_ref.returncode))
    except TimeoutError:
        print(f"[bold]{file}[/bold] [red]Test Failed:[/red] [yellow]Timeout[/yellow]")
        failedtests +=1
        try:
            proc_ref.kill()
            proc_test.kill()
        finally:
            pass
    except Exception as e:
        print(f"[bold]{file}[/bold] [red]Test Failed:[/red] [yellow]Unknown Error[/yellow] {e}")
# Run a tasks
async def main():
    for i in pathlib.Path("craftinginterpreters/test").glob("**/*.lox"):
        testFiles.append(i)
    async with asyncio.TaskGroup() as tg:
        for i in testFiles:
            if i.parents[0].name == "benchmark":
                continue
            task = tg.create_task(run_test_async(i))
            await asyncio.sleep(0.1)
            await task
    print(f"[bold]Total Tests:[/bold] {totaltests} [bold]Passed:[/bold] {passedtests} [bold]Failed:[/bold] {failedtests}")
if __name__ == "__main__":
    asyncio.run(main())

