import { defineConfig } from 'vitepress'

const base = process.env.SITE_BASE || '/'
export default defineConfig({
  lang: 'zh-CN',
  title: '程序设计课堂',
  description: '程序设计课堂知识库：开发环境配置、课堂讲义与 C/C++ 示例源码。',
  base,
  outDir: '../dist',
  srcExclude: ['public/**'],
  cleanUrls: false,
  head: [['link', { rel: 'icon', type: 'image/svg+xml', href: `${base}favicon.svg` }]],
  markdown: {
    math: true,
    image: { lazyLoading: true },
    config(md) {
      const render = md.renderer.rules.html_inline || ((tokens, idx) => tokens[idx].content)
      md.renderer.rules.html_inline = (tokens, idx, options, env, self) => {
        if (tokens[idx].content.startsWith('<img ')) {
          tokens[idx].content = tokens[idx].content.replace(/<img /, '<img loading="lazy" decoding="async" ')
          if (!/\balt=/.test(tokens[idx].content)) {
            const name = tokens[idx].content.match(/src="[^\"]*\/([^/\"]+)\.[a-z]+"/i)?.[1] || '操作步骤截图'
            tokens[idx].content = tokens[idx].content.replace('<img ', `<img alt="${name}" `)
          }
        }
        return render(tokens, idx, options, env, self)
      }
    }
  },
  themeConfig: {
    siteTitle: '程序设计课堂',
    nav: [{ text: '课堂讲义', link: '/' }, { text: '源码资源', link: '/resources' }],
    sidebar: [{ text: '课堂知识库', items: [
      { text: '01 · 编程第一步', link: '/' },
      { text: '示例源码与下载', link: '/resources' }
    ] }],
    outline: { level: [2, 3], label: '本页目录' },
    search: { provider: 'local', options: { locales: { root: { translations: {
      button: { buttonText: '搜索讲义', buttonAriaLabel: '搜索讲义' },
      modal: { noResultsText: '没有找到相关内容', resetButtonTitle: '清空搜索',
        footer: { selectText: '选择', navigateText: '切换', closeText: '关闭' } }
    } } } } },
    docFooter: { prev: '上一篇', next: '下一篇' },
    returnToTopLabel: '回到顶部', sidebarMenuLabel: '讲义目录',
    darkModeSwitchLabel: '切换外观', lightModeSwitchTitle: '切换浅色模式', darkModeSwitchTitle: '切换深色模式',
    skipToContentLabel: '跳转到正文',
    notFound: { title: '没有找到这篇讲义', quote: '可以回到课堂知识库继续阅读。', linkLabel: '返回课堂', linkText: '返回课堂' }
  }
})
