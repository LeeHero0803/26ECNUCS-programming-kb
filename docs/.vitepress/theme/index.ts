import DefaultTheme from 'vitepress/theme'
import { withBase } from 'vitepress'
import './style.css'
export default {
  extends: DefaultTheme,
  enhanceApp({ app }) { app.config.globalProperties.$withBase = withBase }
}
