#ifndef BDN_ViewTextUi_H_
#define BDN_ViewTextUi_H_

#include <bdn/ITextUi.h>
#include <bdn/Window.h>
#include <bdn/ScrollView.h>
#include <bdn/TextView.h>
#include <bdn/ColumnView.h>
#include <bdn/OneShotStateNotifier.h>

namespace bdn
{

    /** ITextUi implementation that uses graphical bdn::View components.

        The UI implementation automatically creates a new top level Window
        when it is initialized.
    */
    class ViewTextUi : public Base, BDN_IMPLEMENTS ITextUi, BDN_IMPLEMENTS ITextSink
    {
      public:
        /** @param uiProvider the UI provider that the text UI should use.
            See the IUiProvider documentation for more information.
            If this is nullptr then the default UI provider for the platform is
           used.*/
        ViewTextUi(IUiProvider *uiProvider = nullptr);

        P<IAsyncOp<String>> readLine() override;

        P<ITextSink> statusOrProblem() override { return this; }

        P<ITextSink> output() override { return this; }

        /** Returns the window that the UI object uses to display the text.*/
        P<Window> getWindow() { return _window; }

      private:
        // ITextSink functions
        void write(const String &s) override;
        void writeLine(const String &s) override;

        bool timerCallback();

        void scrolledSizeChanged();

        void _ensureInitializedWhileMutexLocked();
        void _flushPendingWhileMutexLocked();
        void _doWriteWhileMutexLocked(const String &s);

        P<IUiProvider> _uiProvider;

        Mutex _mutex;
        P<Window> _window;
        P<ScrollView> _scrollView;
        P<ColumnView> _scrolledColumnView;
        P<TextView> _currParagraphView;

        bool _initialized;
        List<String> _pendingList;
        bool _flushPendingScheduled;

        bool _scrollDownPending;
    };
}

#endif
