package com.rusefi;

import org.junit.Test;

import static org.junit.Assert.assertEquals;

public class SignatureHelperTest {
    @Test
    public void test() {
        String url = SignatureHelper.getUrl("rusEFI 2020.07.06.frankenso_na6.2468827536");
        assertEquals("https://rusefi.com/online/ini/rusefi/2020/07/06/frankenso_na6/2468827536.ini", url);
    }
}
