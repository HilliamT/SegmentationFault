#include "display.h"
#include "../query/json_parser.h"
#include <time.h>

/**
 * Local unit test for displaying
 */

int main(int argc, char *argv[]) {

  // Create a question
  Question question;
  question.title = "C segmentation fault: 11";
  question.asker = "user3482695";
  question.body = "<pre><code>#include &lt;stdio.h&gt;\nint main()\n{\n    char memf[10000];\n    int memlen, vn , vw,vh,fattime , posit , speed , interval , nattacks,time=0,qa=0,ninit=0,elxan;\n    scanf(\"%d\\n\",&amp;memlen);\n    scanf(\"%d\\n\",&amp;vn);\n    scanf(\"%d %d %d %d %d %d %d\",&amp;vw,&amp;vh,&amp;fattime,&amp;posit,&amp;speed,&amp;interval,&amp;nattacks);\n\n        while(nattacks&gt;ninit)\n        {\n            if(time==fattime)\n            {\n                for(elxan=posit;elxan&lt;=posit+vw;elxan=posit++)\n                {\n                    memf[elxan]=vh;\n                }\n                posit=posit+speed;\n                time++;\n                ninit++;\n\n            }\n            else if(time&gt;fattime)\n             {\n                for(qa=0;qa&lt;100000;qa++)\n                {\n                   if(ninit==nattacks)\n                       break;\n                   else if(qa%interval==0)\n                   {\n                       for(elxan=posit;elxan&lt;=posit+vw;elxan=posit++)\n                       {\n                           memf[elxan]=vh;\n                       }\n                       posit=posit+speed;\n                       time++;\n                       ninit++;\n                   }\n\n\n                   else\n                       posit=posit+speed;\n                  }\n             }\n             else\n             {  time++;\n                posit=posit+speed;\n\n             }\n        }\n    /*for(px=0;px&lt;=memlen;px++)\n    {\n        if(memf[px]=='0')\n            memf[px]=1;\n    }*/\n\n\n\n    printf(\"%s\",memf);\n    return 0;\n}\n</code></pre>\n\n<p>I get <code>segmentation fault:11</code> while executing this code. I know it has something to do with arrays. What is the mistake?</p>\n";
  question.score = 0;
  question.creation_date = 1396298275;
  question.answer_count = 2;
  question.answers = linked_list_make();

  // Create some answers
  Answer answer1;
  answer1.answerer = "RBerteig";
  answer1.body = "<p>I think this loop is almost guaranteed to run away and access memory outside of the array <code>memf</code>, even assuming that the entered initial values are reasonable.</p>\n\n<pre><code>for(elxan=posit;elxan&lt;=posit+vw;elxan=posit++) {  \n    memf[elxan]=vh; \n}\n</code></pre>\n\n<p>Each iteration tests against <code>posit+vw</code>, but each iteration also increments <code>posit</code> while keeping <code>elxan</code> set to <code>posit</code>'s previous value, which unless pathological values for <code>posit</code> and <code>vw</code> are entered, is guaranteed to make the condition always true, and the loop will not terminate within <code>sizeof(memf)</code> iterations.</p>\n\n<p>Once <code>elxan</code> is large enough, accessing memory outside of the declared size of the array is going to cause trouble. While in general you have invoked \"undefined behavior\" after which nearly anything can happen, the specific symptom of a segmentation fault is very consistent with what we would expect to see eventually from overrunning an array bounds.</p>\n";
  answer1.is_accepted = true;
  answer1.score = 2;
  answer1.creation_date = 1396298846;
  linked_list_append(question.answers, &answer1);

  Answer answer2;
  answer2.answerer = "S&#248;ren Debois";
  answer2.body = "<p>Here's how to deal with such an error:</p>\n\n<p>First, add <code>#include &lt;assert.h&gt;</code>. Then, whenever you read from or assign to an array, test first that you are indexing within bounds. E.g., this line:</p>\n\n<pre><code>memf[elxan]=vh;\n</code></pre>\n\n<p>Should be changed to these lines:</p>\n\n<pre><code>assert(0 &lt;= elxan &amp;&amp; elxan &lt; 10000);\nmemf[elxan]=vh;\n</code></pre>\n\n<p>This will help you pin-point the error. </p>\n\n<hr>\n\n<p>You'll notice that you usually don't see so many <code>asserts</code> in other C-programs. That's ok; once you get used to writing C, you'll find you can safely let some of them go. For starters, I think you'll find them very helpful. </p>\n";
  answer2.is_accepted = false;
  answer2.score = 2;
  answer2.creation_date = 1396298704;
  linked_list_append(question.answers, &answer2);

  // Display question, with its answers
  display_question(&question);
  linked_list_destroy(question.answers);
  return 0;
}
